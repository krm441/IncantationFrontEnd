#pragma once
#include "Token.h"
#include <stack>

namespace Incantation
{
	class Lexer
	{
	public:
		Lexer(const std::string& in);
		Tokens LexAll();
		void LexStep();
		void CheckBracketSolidity();
	private:
		void Advance(size_t advance = 1);
		char Peek();
		char PeekNext();
		void Dump();
		void Add();
		size_t mIndex;
		std::string mBuffer;
		std::string mTemp;
		Tokens mRet;
		int mLine;
		std::stack<int> mStack;
	};
}