#include "Core/pchInc.h"
#include "RDP.h"
#include "VerboseAST.h"

using namespace Incantation;

Incantation::RDP::RDP(const Tokens& in) : mIterator(0), mBuffer(in)
{
}

void Incantation::RDP::Parse()
{
	// Insert End Of File
	mBuffer.push_back(Token("<<EoF>>", mBuffer.back().LineCount()));

	mMain = ParseBody();
}

void Incantation::RDP::Verbose()
{
	if (mMain)
	{
		AstNode plack = mMain;
		std::visit(Visitor{}, plack);
	}
}

Ptr<TestStatement> Incantation::RDP::ParseTest()
{
	auto ret = create<TestStatement>();
	ret->test = ParseOR();
	return ret;
}

Ptr<IfStatement> Incantation::RDP::ParseIfSt(const CodeBlockSettings& settings)
{
	auto ret = create<IfStatement>();
	Expect("if");
	Advance();
	Expect("(");
	Advance();
	ret->test = ParseTest();
	Expect(")");
	Advance();
	ret->consequent = ParseCodeBlock(settings);
	Expect("}");
	if (PeekNext_if("else"))
	{
		Advance(); // Eat the '}'
		Advance(); // Eat the 'else'
		if (Peek().Type() == TokenType::If)
		{
			ret->alternate = ParseIfSt(settings);
		}
		else
		{
			ret->alternate = ParseCodeBlock(settings);
			Expect("}");
		}
	}
	else
	{
		ret->alternate = create<NullPtr>();
	}
	return ret;
}

Ptr<WhileStatement> Incantation::RDP::ParseWhileSt(const CodeBlockSettings& settings)
{
	auto ret = create<WhileStatement>();
	Advance();
	Advance(); // eat '('
	ret->test = ParseTest();
	Expect(")");
	Advance();
	CodeBlockSettings set;
	set.FunctionScope = settings.FunctionScope;
	set.GlobalScope = 0;
	set.LoopScope = 1;
	set.previous = settings.previous;
	ret->body = ParseCodeBlock(set);
	Expect("}");
	Advance();
	return ret;
}

Ptr<ForStatement> Incantation::RDP::ParseForSt(const CodeBlockSettings& settings)
{
	throw "For statements are not parsable yet - construction in process";
	return Ptr<ForStatement>();
}

Ptr<FunctionExpression> Incantation::RDP::ParseFunctionDecl(const CodeBlockSettings& settings)
{
	auto functionCall = create<FunctionExpression>();

	Advance(); // Eat the 'function' keyword

	if (Peek().Type() != TokenType::Identifier)
	{
		throw SyntaxError("wrong declaration of function:: \'" + Peek().Value() + "\', line:: " + std::to_string(Peek().LineCount()));
	}

	functionCall->id = ParseIdentExprStrict();

	functionCall->params = ParseParamsList();

	CodeBlockSettings set;
	set.FunctionScope = 1;
	set.GlobalScope = 0;
	set.LoopScope = 0;
	set.previous = settings.previous;

	functionCall->block = ParseCodeBlock(set);

	return functionCall;
}

Ptr<VarDeclaration> Incantation::RDP::ParseVarDeclaration()
{
	auto ret = ::create<VarDeclaration>();

	Advance();
	ret->name = ParseIdentExprStrict();
	Advance();
	if (Peek().Type() == TokenType::Init)
	{
		// Eat '='
		Advance(-1);
		auto ass = ParseAssignment();
		ret->expression = ass;
		ret->expression->left = ass->left;
		ret->expression->Op = ass->Op;
		ret->expression->right = ass->right;
		Expect(";");
		return ret;
	}
	Expect(";");
	return ret;
}

Ptr<ParamsList> Incantation::RDP::ParseParamsList()
{
	auto ret = ::create<ParamsList>();
	Advance();
	Expect("(");
	while (true)
	{
		Advance();

		// Early return
		if (Peek().Type() == TokenType::RRbr)
		{
			// eat it
			Advance();
			break;
		}

		ret->params.push_back(ParseIdentExprStrict());
		Advance();
		if (Peek().Type() == TokenType::Comma)
		{
			continue;
		}
		else if (Peek().Type() == TokenType::RRbr)
		{
			// eat it
			Advance();
			break;
		}
		else throw SyntaxError("Unexpected identifier:: ");
	}

	return ret;
}

Ptr<IdentifierExpression> Incantation::RDP::ParseIdentExprStrict()
{
	auto ret = ::create<IdentifierExpression>();

	if (Peek().Type() != TokenType::Identifier)
	{
		throw SyntaxError("Incorrect parameter pack:: " + Peek().Value());
	}
	ret->id = Peek();

	return ret;
}

Ptr<ReturnStatement> Incantation::RDP::ParseReturnStatement()
{

	auto ret = ::create<ReturnStatement>();

	Advance(); // eat 'return'
	if (Peek().Type() == TokenType::SemiCol)
	{
		ret->argument = std::shared_ptr<NullPtr>();
		return ret;
	}
	else
	{
		ret->argument = ParseOR();
	}

	return ret;
}

Ptr<AssignmentExpression> Incantation::RDP::ParseAssignment()
{
	auto ret = ::create<AssignmentExpression>();

	if (Peek().Type() == TokenType::Identifier && PeekNext_if("="))
	{
		ret->left = ParseIdentExprStrict();
		Advance();
		ret->Op = OperatorType::ASS;
		Advance();
		ret->right = ParseOR();
		return ret;
	}

	throw SyntaxError("Unexpected token in assignment operation");
}

std::ostream& Incantation::operator<<(std::ostream& os, const OperatorType& value)
{
	switch (value)
	{
	case OperatorType::Null:os << "NOP";break;
	case OperatorType::ASS:	os << "=";	break;
	case OperatorType::ADD:	os << "+";	break;
	case OperatorType::SUB:	os << "-";	break;
	case OperatorType::MUL:	os << "*";	break;
	case OperatorType::DIV:	os << "/";	break;
	case OperatorType::MOD:	os << "%";	break;
		//<, >, <=, >=, ==, !=
	case OperatorType::Less: os << '<'; break;
	case OperatorType::Greater: os << '>'; break;
	case OperatorType::LessEqual: os << "<="; break;
	case OperatorType::GreaterEqual: os << ">="; break;
	case OperatorType::Equal: os << "=="; break;
	case OperatorType::NotEqual: os << "!="; break;
	default: break;
	}
	return os;
}
