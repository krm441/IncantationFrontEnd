#include "Core/IncFrontPCH.h"
#include "Token.h"
#include "Core/SafeCall.h"
#include "Logger/LOG.h"

// NAME CHECKS
bool IsId(const std::string& str)
{
	static std::regex pattern("^[[:alpha:]]");  // Regex pattern: starts with not a number

	if (std::regex_search(str, pattern)) 
	{
		return true;
	}
	return false;
}

bool IsNum(const std::string& str)
{
	std::istringstream iss(str);
	double number;
	iss >> std::noskipws >> number;
	return iss.eof() && !iss.fail();
}

static bool IsOperator(const std::string& str)
{
	if (str.size() > 1)
		return false;

	static std::unordered_set<char> set
	{
		'+', '-', '*', '/', '%', '<', '>', '!'
	};
	if (set.find(str.front()) != set.end())
	{
		return true;
	}
	return false;
}

// TOKEN CPP

Incantation::Token::Token() : value(), line_count(0), type(TokenType::Null)
{
}

Incantation::Token::Token(const std::string& value, int count) :
	value(value), line_count(count), type(TokenType::Null)
{
	static std::unordered_map<std::string, TokenType> sMap
	{
		{"<<EoF>>",		TokenType::EoF},
		{"||",			TokenType::LogicalOperator},
		{"&&",			TokenType::LogicalOperator},
		{"!",			TokenType::LogicalOperator},
		{"return",		TokenType::Return},
		{"break",		TokenType::Break},
		{"if",			TokenType::If},
		{"while",		TokenType::Loop},
		{"for",			TokenType::Loop},
		{"<<endl>>",	TokenType::Endl},
		{";",			TokenType::SemiCol},
		{",",			TokenType::Comma},
		{"!=",			TokenType::Operator},
		{"==",			TokenType::Operator},
		{"<=",			TokenType::Operator},
		{">=",			TokenType::Operator},
		{"=",			TokenType::Init},
		{"function",	TokenType::Func},
		{"class",		TokenType::Class},
		{"var",			TokenType::Var},
		{"{",			TokenType::LCBr},
		{"}",			TokenType::RCBr},
		{"(",			TokenType::LRbr},
		{")",			TokenType::RRbr},
		{"[",			TokenType::LSbr},
		{"]",			TokenType::RSbr},
	};

	if (sMap.find(value) != sMap.end())
	{
		type = sMap.at(value);
	}
	else if ((value.front() == '\"' && value.back() == '\"') || (value.front() == '\'' && value.back() == '\''))
	{
		this->value.erase(this->value.begin());
		this->value.pop_back();
		type = TokenType::String;
	}
	else if (value == "true")
	{
		type = TokenType::Constant;
		const_value = 1;
	}
	else if (value == "false")
	{
		type = TokenType::Constant;
		const_value = 0;
	}
	else if (value.substr(0, 5) == "<<var" || value.substr(0, 5) == "<<ret")
		type = TokenType::Identifier;
	else if (IsOperator(value))
		type = TokenType::Operator;
	else if (IsId(value))
		type = TokenType::Identifier;
	else if (IsNum(value))
	{
		type = TokenType::Constant;
		const_value = std::stod(value);
	}
	else
	throw StopCallInc("Unsupported token: value: \'" + value + '\'' + ", on line: " + std::to_string(LineCount()));
}

std::ostream& Incantation::operator<<(std::ostream& os, TokenType& type)
{
	switch (type)
	{
	case Incantation::TokenType::Null: os << "Null";
		break;
	case Incantation::TokenType::SemiCol: os << "Semicolon";
		break;
	case Incantation::TokenType::Init: os << "Init";
		break;
	case Incantation::TokenType::Operator: os << "Operator";
		break;
	case Incantation::TokenType::LogicalOperator: os << "Logical Opr";
		break;
	case Incantation::TokenType::Return: os << "Return";
		break;
	case Incantation::TokenType::LCBr: os << "LCBr";
		break;
	case Incantation::TokenType::RCBr: os << "RCBr";
		break;
	case Incantation::TokenType::LRbr: os << "LRbr";
		break;
	case Incantation::TokenType::RRbr:  os << "RRbr";
		break;
	case Incantation::TokenType::Func: os << "Func";
		break;
	case Incantation::TokenType::Class: os << "Class";
		break;
	case Incantation::TokenType::Comma: os << "Comma";
		break;
	case Incantation::TokenType::Var: os << "Var";
		break;
	case Incantation::TokenType::Identifier:  os << "Identifier";
		break;
	case Incantation::TokenType::Constant: os << "Constant";
		break;
	case Incantation::TokenType::String: os << "String";
		break;
	case Incantation::TokenType::If: os << "If";
		break;
	case Incantation::TokenType::Loop: os << "Loop";
		break;
	case Incantation::TokenType::Endl: os << "Endl";
		break;
	case Incantation::TokenType::EoF: os << "EoF";
		break;
	default:
		break;
	}

	return os;
}

std::ostream& Incantation::operator<<(std::ostream& os, Token& value)
{
	os << "Token:: Type: " << value.Type() << " , Value: " << value.Value() << '\n';
	return os;
}

std::ostream& Incantation::operator<<(std::ostream& os, Tokens& value)
{
	for (auto& t : value)
		os << t;

	return os;
}
