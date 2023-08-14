#pragma once
#include "Lexer/Token.h"
#include "Core/SafeCall.h"
#include "Logger/LOG.h"
#include "Core/Ref.h"
#include "ASTnodes.h"

//break statement
//Remove dump
//for statement
//array

namespace Incantation
{
	/// <summary>
	/// Recursive descent parser
	/// </summary>
	class RDP
	{
	public:
		RDP(const Tokens& in);

		void Parse();

		void Verbose();

		Ptr<Program> GetAST() { if (mMain) return mMain; throw "program not compiled"; }

	private:
		Ptr<Program> mMain;
		Tokens mBuffer;
		size_t mIterator;
		inline Token& LookAhead(Tokens& in) 
		{
			if (mIterator + 1 <= mBuffer.size()) { return mBuffer.at(mIterator + 1); }
			else throw StopCallInc("Next token outa reach");
		};

		inline void Advance(int distance = 1)
		{
			mIterator+=distance;
		};
		template<typename... Args, typename = std::enable_if_t<(... && std::is_convertible_v<Args, std::string>)>>
		inline void Expect(Args ...expect)
		{
			bool match = ((Peek().Value() == expect) || ...);
			if (!match)
			{
				std::string expected_values;
				((expected_values += '\'' + std::string(expect) + '\'' + ", or: "), ...);
				expected_values = expected_values.substr(0, expected_values.size() - 5); // remove last ', or: '
				throw SyntaxError("Unexpected token, line:: " + std::to_string(Peek().LineCount()) + " , expected:: " + expected_values + " received:: " + Peek().Value() );
			}
		}
		inline Token& Peek()
		{
			if (mIterator < mBuffer.size()) { return mBuffer.at(mIterator); }
			else throw StopCallInc("Next token outa reach");
		}
		inline void LogPeek()
		{
			if (mIterator <= mBuffer.size()) { console.log(mBuffer.at(mIterator).Value()); }
			else throw StopCallInc("Next token outa reach");
		}
		inline bool PeekNext_if(const std::string& t)
		{
			if (mIterator + 1 < mBuffer.size()) { return mBuffer.at(mIterator + 1).Value() == t; }
			else return false;
		}
		inline Token& PeekNext()
		{
			if (mIterator + 1 == mBuffer.size())
				throw SyntaxError("Unexpected EndOfFile::EoF during peek next");
			return mBuffer.at(mIterator + 1);
		}
		inline Token& PeekPrev()
		{
			if (mIterator - 1 <= 0)
				throw SyntaxError("Unable to peek previous");
			return mBuffer.at(mIterator - 1);
		}

		inline bool FollowsPatternToken(const std::vector<TokenType>& in)
		{
			int AdvanceIndex = 0;
			bool ret = true;
			for (size_t i = 0; i < in.size(); i++)
			{
				if (Peek().Type() != in.at(i))
				{
					ret = false;
					break;
				}
				Advance();
				AdvanceIndex++;
			}
			if (ret == false)
			{
				Advance(-AdvanceIndex);
				return false;
			}
			return true;
		}
		inline bool FollowsPatternString(const std::vector<std::string>& in)
		{
			int AdvanceIndex = 0;
			bool ret = true;
			for (size_t i = 0; i < in.size(); i++)
			{
				if (Peek().Value() != in.at(i))
				{
					ret = false;
					break;
				}
				Advance();
				AdvanceIndex++;
			}
			if (ret == false)
			{
				Advance(-AdvanceIndex);
				return false;
			}
			else
			{
				Advance(-AdvanceIndex);
				return true;
			}
		}

		// Parsing Rules - Short version
		/*
		body					::= code_block
		code_block				::= var_declaration | function_declaration | if_statement | loop | expression 
		var_declaration			::= 'var' expression
		function_declaration	::= 'function' identifier '(' arg_list ')' body
		if_statement			::= 'if' '(' test ')' body consecutive ('else' alternate) *
		loop					::= 'while' | 'for' test body
		expression				::= identifier | assignment_expression
		assignment_expression	::= '=' identifier | literal | binary_expression
		*/

		Ptr<Program> ParseBody();
		AstNode ParseAND();
		AstNode ParseOR();
		AstNode ParseCompare();
		AstNode ParseAddSub();
		AstNode ParseMulDiv();
		AstNode ParseUnary();

		Ptr<TestStatement> ParseTest();
		Ptr<IfStatement> ParseIfSt(const CodeBlockSettings& settings);
		Ptr<WhileStatement> ParseWhileSt(const CodeBlockSettings& settings);
		Ptr<ForStatement> ParseForSt(const CodeBlockSettings& settings);
		Ptr<FunctionExpression> ParseFunctionDecl(const CodeBlockSettings& settings);
		Ptr<VarDeclaration> ParseVarDeclaration();
		Ptr<ParamsList> ParseParamsList();
		Ptr<IdentifierExpression> ParseIdentExprStrict();
		Ptr<BlockStatement> ParseCodeBlock(const CodeBlockSettings& settings);
		Ptr<ReturnStatement> ParseReturnStatement();
		Ptr<AssignmentExpression> ParseAssignment();

		// Logger
		Console console;
	};
}