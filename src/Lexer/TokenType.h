#pragma once

namespace Incantation
{
	enum class TokenType
	{
		Null = 0, SemiCol,
		Init, Operator, LogicalOperator,
		Return,
		LCBr, RCBr,
		LRbr, RRbr,
		LSbr, RSbr,
		Func, Class,
		Comma,
		Var,
		Identifier,
		Constant,
		If,
		Loop,
		Break,
		String,
		Endl,
		EoF
	};
}