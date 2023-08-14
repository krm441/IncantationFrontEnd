#pragma once
#include "TokenType.h"
#include <string>
#include <vector>

namespace Incantation
{
	struct Token
	{
		Token();
		Token(const std::string& value, int count);
		TokenType& Type() { return type; }
		std::string& Value() { return value; }
		int LineCount() {return static_cast<int>(line_count);};
		double ConstValue() { return const_value; }
	private:
		std::string value;
		TokenType type;
		double line_count;
		double const_value = 0.0;
	};

	using Tokens = std::vector<Token>;

	std::ostream& operator<<(std::ostream& os, TokenType& type);
	std::ostream& operator<<(std::ostream& os, Token& value);
	std::ostream& operator<<(std::ostream& os, Tokens& value);
}