#include "Core/IncFrontPCH.h"
#include "Parser/RDP.h"
#include "Logger/LOG.h"

using namespace Incantation;

AstNode Incantation::RDP::ParseOR()
{
	auto left = ParseAND();
	while (true)
	{
		if (Peek().Value() == "||")
		{
			Advance(); // eat 
			auto ret = create<LogicalExpression>();
			ret->left = left;
			ret->op = LogicalOperator::OR;
			ret->right = ParseOR();
			left = ret;
		}
		else break;
	}
	return left;
}

AstNode Incantation::RDP::ParseAND()
{
	auto left = ParseCompare();
	while (true)
	{
		if (Peek().Value() == "&&")
		{
			Advance(); // eat 
			auto ret = create<LogicalExpression>();
			ret->left = left;
			ret->op = LogicalOperator::AND;
			ret->right = ParseOR();
			left = ret;
		}
		else return left;
	}
	return left;
}

AstNode Incantation::RDP::ParseCompare()
{
	auto left = ParseAddSub();
	
	while (true)
	{
		auto Char = Peek().Value().front();
		OperatorType Op = OperatorType::Null;

		// Special case
		if (Peek().Value() == "==")
			Op = OperatorType::Equal;
		else if (Peek().Value() == "!=")
			Op = OperatorType::NotEqual;
		else if (Peek().Value() == "<=")
			Op = OperatorType::LessEqual;
		else if (Peek().Value() == ">=")
			Op = OperatorType::GreaterEqual;
		else if (Peek().Value() == "<")
			Op = OperatorType::Less;
		else if (Peek().Value() == ">")
			Op = OperatorType::Greater;
		else
			return left;
		Advance();
		auto right = ParseOR();
		auto ret = ::create<BinaryExpression>();
		ret->left = left;
		ret->op = Op;
		ret->right = right;
		left = ret;
	}
	return left;
}

AstNode Incantation::RDP::ParseAddSub()
{
	auto left = ParseMulDiv();
	while (true)
	{
		auto Char = Peek().Value().front();
		OperatorType Op = OperatorType::Null;
		switch (Char)
		{
		case '+': Op = OperatorType::ADD;
			break;
		case '-': Op = OperatorType::SUB;
			break;
		default:
			return left;
			break;
		}
		Advance();
		auto right = ParseOR();
		auto ret = ::create<BinaryExpression>();
		ret->left = left;
		ret->op = Op;
		ret->right = right;
		left = ret;
	}

	return left;
}

AstNode Incantation::RDP::ParseMulDiv()
{
	auto left = ParseUnary();
	while (true)
	{
		auto Char = Peek().Value().front();
		OperatorType Op = OperatorType::Null;
		switch (Char)
		{
		case '*': Op = OperatorType::MUL;
			break;
		case '/': Op = OperatorType::DIV;
			break;
		case '%': Op = OperatorType::MOD;
			break;
		default: return left;
			break;
		}

		Advance();
		auto right = ParseOR();

		auto ret = ::create<BinaryExpression>();
		ret->left = left;
		ret->op = Op;
		ret->right = right;
		left = ret;
	}
	return left;
}

AstNode Incantation::RDP::ParseUnary()
{
	bool negate = false; bool increment = false;
	// Case - 5;
	if (Peek().Type() == TokenType::Operator)
	{
		auto op = Peek().Value().front();
		switch (op)
		{
		case '-': negate = true;
			break;
		case '+': increment = true;
			break;
		default:
			throw SyntaxError("Invalid Unary operation:: " + Peek().Value() + ' ' + std::to_string(Peek().LineCount()));
			break;
		}
		Advance();
		if (Peek().Type() == TokenType::Operator)
		{
			auto op = Peek().Value().front();
			switch (op)
			{
			case '-': if (!negate)
				throw SyntaxError("Invalid Unary operation:: " + Peek().Value() + ' ' + std::to_string(Peek().LineCount()));
				break;
			case '+': if (!increment)
				throw SyntaxError("Invalid Unary operation:: " + Peek().Value() + ' ' + std::to_string(Peek().LineCount()));
				break;
			default:
				throw SyntaxError("Invalid Unary operation:: " + Peek().Value() + ' ' + std::to_string(Peek().LineCount()));
				break;
			}
			Advance(); //clear
			if (Peek().Type() != TokenType::Identifier)
			{
				throw SyntaxError("Invalid Unary operation:: " + Peek().Value() + ' ' + std::to_string(Peek().LineCount()));
			}
			auto ret = create<UpdateExpression>();
			if (negate)
				ret->type = UnaryExpressionType::UpdatePrefixDecrement;
			else if (increment)
				ret->type = UnaryExpressionType::UpdatePrefixIncrement;
			ret->argument = ParseIdentExprStrict();
			return ret;
		}
		else if (Peek().Type() == TokenType::Identifier)
		{
			auto ret = create<UnaryExpression>();

			// here it could be a function call
			if (PeekNext_if("("))
			{
				auto arg = create<FunctionCall>();
				arg->id = Peek();
				Advance(); // eat the name;
				Advance(); // eat '(';
				if (Peek().Type() == TokenType::RRbr)
				{
					// no args
					Advance();

					if (!negate)
						throw SyntaxError("Not negate ");
					ret->type = UnaryExpressionType::Negate;

					ret->argument = arg;

					return ret;
				}

				while (true)
				{
					arg->args.push_back(ParseOR());
					if (Peek().Type() == TokenType::Comma)
					{
						Advance();
						continue;
					}
					else if (Peek().Type() == TokenType::RRbr)
					{
						Advance();
				
						if (!negate)
							throw SyntaxError("Not negate ");
						ret->type = UnaryExpressionType::Negate;

						ret->argument = arg;

						return ret;
					}
				}
			}

			ret->type = UnaryExpressionType::Negate;
			auto arg = create<IdentifierExpression>();
			arg->id = Peek();
			ret->argument = arg;
			Advance();
			return ret;
		}
		else if (Peek().Type() == TokenType::Constant)
		{
			auto ret = create<UnaryExpression>();
			ret->type = UnaryExpressionType::Negate;
			auto arg = create<Literal>(Peek().ConstValue());
			ret->argument = arg;
			Advance();
			return ret;
		}
		else if (Peek().Type() == TokenType::LRbr)
		{
			auto ret = create<UnaryExpression>();
			ret->type = UnaryExpressionType::Negate;
			Advance();
			ret->argument = ParseOR();
			Expect(")");
			Advance();
			return ret;
		}
		throw SyntaxError("Invalid Unary operation:: " + Peek().Value() + ' ' + std::to_string(Peek().LineCount()));
	}
	else if (Peek().Type() == TokenType::LogicalOperator)
	{
		assert(Peek().Value().front() == '!' && "wrong unary not logic");

		auto un = create<UnaryExpression>();
		un->type = UnaryExpressionType::NOT;
		Advance(); // Eat ' ! '
		un->argument = ParseUnary();

		return un;
	}
	else if (Peek().Type() == TokenType::Identifier)
	{
		Advance();

		// Case ++ | --
		if (Peek().Type() == TokenType::Operator && PeekNext().Type() == TokenType::Operator)
		{
			if (FollowsPatternString({ "+", "+" }))
			{
				auto ret = create<UpdateExpression>();
				Advance(-1);
				ret->argument = ParseIdentExprStrict();
				ret->type = UnaryExpressionType::UpdateSuffixIncrement;
				Advance(3);
				return ret;

			}
			else if (FollowsPatternString({ "-", "-" }))
			{
				auto ret = create<UpdateExpression>();
				Advance(-1);
				ret->argument = ParseIdentExprStrict();
				ret->type = UnaryExpressionType::UpdateSuffixDecrement;
				Advance(3);
				return ret;
			}
			throw SyntaxError("Incorrect Update expression detected: line: " + std::to_string(Peek().LineCount()));
		}
		// Case Function Call
		else if (Peek().Type() == TokenType::LRbr)
		{
			auto ret = create<FunctionCall>();

			Advance(-1);
			ret->id = Peek();
			Advance();
			Advance();
			// if no args
			if (Peek().Type() == TokenType::RRbr)
			{
				Advance(); // Eat bracket only
				return ret;
			}

			while (true)
			{
				ret->args.push_back(ParseOR());
				if (Peek().Type() == TokenType::Comma)
				{
					Advance();
					continue;
				}
				else if (Peek().Type() == TokenType::RRbr)
				{
					Advance();
					return ret;
				}
				assert(false);
			}
		}
		Advance(-1);

		auto ret = ::create<IdentifierExpression>();
		ret->id = Peek();
		Advance();
		return ret;
	}
	else if (Peek().Type() == TokenType::Constant)
	{
		auto ret = ::create<Literal>(Peek().ConstValue());
		Advance();
		return ret;
	}
	else if (Peek().Type() == TokenType::String)
	{
		auto ret = create<String>(Peek().Value());
		Advance();
		return ret;
	}
	else if (Peek().Type() == TokenType::LRbr)
	{
		Advance(); // Eat the '('
		auto ret = ParseOR();
		Expect(")");
		Advance();
		return ret;
	}
	else if (Peek().Type() == TokenType::LSbr)
	{
		auto arr = create<Array>();

		Advance(); // eat '['
		while (true)
		{
			if (Peek().Type() == TokenType::RSbr)
			{
				Advance(); // eat the ']'
				Expect(";");
				return arr;
			}
			else if (Peek().Type() == TokenType::Comma)
			{
				Advance(); // eat the ','
			}
			arr->items.push_back(ParseOR());
		}
		throw SyntaxError("array error", Peek().Value());
	}
	throw SyntaxError("Unexpected token. Expected identifier but got: ", Peek().Type(), ", at line: ", Peek().LineCount());
}