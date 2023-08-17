#include "Core/pchInc.h"
#include "Parser/RDP.h"
#include "Parser/Vault.h"
#include "Logger/LOG.h"

using namespace Incantation;

Ptr<Program> Incantation::RDP::ParseBody()
{
	auto main = create<Program>();

	CodeBlockSettings settings;
	settings.GlobalScope = 1;
	main->block = ParseCodeBlock(settings);

	return main;
}

Ptr<BlockStatement> Incantation::RDP::ParseCodeBlock(const CodeBlockSettings& settings)
{

	auto block = create<BlockStatement>();
	block->vault = create<Vault>(settings.previous);

	if (!settings.GlobalScope)
	{
		Expect("{");
		Advance();
	}
	else
	{
		settings.previous = block->vault;
	}

	SafeWhileLoop loop("Parsing Code Block");
	while (loop.True())
	{
		auto type = Peek().Type();
		switch (type)
		{
		case Incantation::TokenType::Return:
		{
			if (settings.FunctionScope)
			{
				block->children.push_back(ParseReturnStatement());
				Expect(";");
				Advance();
			}
			else
				throw SyntaxError("Return statement used outside of function. Line: " + std::to_string(Peek().LineCount()));
		}
			break;
		case Incantation::TokenType::LCBr:
		{
			block->children.push_back(ParseCodeBlock(settings));
			Expect("}");
			Advance();
		}
			break;
		case Incantation::TokenType::RCBr:
			loop.Stop();
			break;
		case Incantation::TokenType::Func:
		{

			auto func = ParseFunctionDecl(settings);

			// Now add to vault
			block->vault->AddFunction(func);
			block->children.push_back(func);
			Expect("}");
			Advance();
		}
			break;
		case Incantation::TokenType::Var:
		{
			auto ret = ParseVarDeclaration();
			block->children.push_back(ret);
			block->vault->AddVar(ret->name);
			Expect(";");
			Advance();
		}
			break;
		case Incantation::TokenType::Identifier:
		{
			if (Peek().Type() == TokenType::Identifier && PeekNext_if("="))
			{
				block->children.push_back(ParseAssignment());
			}
			else
			{
				block->children.push_back(ParseUnary());
			}
			Expect(";");// , "<<EoF>>", "<<endl>>");
			Advance();
		}
			break;
		case Incantation::TokenType::If:
		{
			CodeBlockSettings set = settings;
			assert(set.FunctionScope == settings.FunctionScope && set.GlobalScope == settings.GlobalScope && set.LoopScope == settings.LoopScope);
			set.GlobalScope = 0;
			block->children.push_back(ParseIfSt(set));
			Expect("}");
			Advance();
		}
			break;
		case Incantation::TokenType::Loop:
		{
			if (Peek().Value() == "while")
				block->children.push_back(ParseWhileSt(settings));
			else if (Peek().Value() == "for")
				block->children.push_back(ParseForSt(settings));
		}
			break;
		case Incantation::TokenType::Break:
			if (settings.LoopScope)
			{
				block->children.push_back(create<BreakStatement>());
				Advance();
				Expect(";");
				Advance();
			}
			else
				throw SyntaxError("Break statement used outside of loop or switch. On line:", Peek().LineCount());
			break;
		case Incantation::TokenType::EoF:
			loop.Stop();
			break;
		default:
			throw SyntaxError("Body::Unexpected token: " + Peek().Value(), "at line: ", Peek().LineCount());
			break;
		}
	}
	if (!settings.GlobalScope)
		Expect("}");
	return block;
}