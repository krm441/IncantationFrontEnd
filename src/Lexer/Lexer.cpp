#include "Core/IncFrontPCH.h"
#include "Lexer.h"
#include "Core/SafeCall.h"
#include "Logger/LOG.h"

using namespace Incantation;

Incantation::Lexer::Lexer(const std::string& in) : mIndex(0), mLine(1), mBuffer(in)
{
}

Tokens Incantation::Lexer::LexAll()
{
	if (mBuffer.empty()) // Early return
		throw StopCallInc("File is empty");

	while (mIndex < mBuffer.size())
	{
		LexStep();
	}

	CheckBracketSolidity();

	return mRet;
}

std::unordered_set<char> separs
{
	'(', ')', '{', '}', '[', ']', ',', ' ', '!', '=',
	';', '-', '+', '*', '/', '%', '\"', '<', '>'
};

void Lexer::LexStep()
{
	auto current_char = Peek();
	switch (current_char)
	{
	case '(':	case '[':	case '{':
		mStack.push(mLine);
	break;
	case ')':	case ']':	case '}':
	{
		if (mStack.empty())
			throw SyntaxError("Possible brackets mismatch located, at line: ", mLine);
		mStack.pop();
	}
	break;
	case '<':	case '>':	case '!':	case '=':
	{
		Add();
		Advance();
		if (Peek() == '=')
		{
			Add();
			Advance();
			Dump();
			return;
		}
		else
		{
			Dump();
			return;
		}
		assert(false);
	}
	break;
	case '\"':	case '\'':
	{
		int beginLine = mLine;
		char openning_char = current_char;
		Add();
		Advance();
		SafeWhileLoop loop;
		while (loop.True())
		{
			// error
			if (mIndex == mBuffer.size() || Peek() == '\n')
				throw SyntaxError("Strig began at line: ", beginLine, "but never ended");
			// exit
			if (Peek() == openning_char)
				break;
			Add();
			Advance();
		}
		Add(); // eat the ' " ' or ' ' '
		Dump();
		Advance();
		return;
	}
	break;
	case '/':
	{
		if (PeekNext() == '/')
		{
			Advance();
			Dump();
			while (Peek() != '\n')
			{
				Advance();
			}
			mLine++;
			Advance();
			return;
		}
		else if (PeekNext() == '*')
		{
			Advance();
			Dump();

			while (true)
			{
				if (Peek() == '*' && PeekNext() == '/')
				{
					break;
				}
				else if (Peek() == '\n')
				{
					mLine++;
				}

				Advance();
			}
			Advance();
			Advance();
			return;
		}
		else
		{
			Dump(); // previous
			Add(); // add the '/'
			Dump();
			Advance();
			return;
		}
	}
	break;
	case ' ':	case '\t':
		Dump();
		Advance();
		return;
	break;
	case '\n':
		Dump();
		//mRet.push_back(Token("<<endl>>", mLine++));
		mLine++;
		Advance();
		return;
	break;

	default:
		break;
	}

	if (separs.find(Peek()) != separs.end())
	{
		Dump();
		Add();
		Dump();
		Advance();
		return;
	}
	Add();
	Advance();
}

void Incantation::Lexer::CheckBracketSolidity()
{
	if (!mStack.empty())
		throw SyntaxError("Opening and closing brackets mismatch error. Unmatched closing bracket at line:", mStack.top());
}

void Incantation::Lexer::Advance(size_t advance)
{
	mIndex += advance;
}

char Incantation::Lexer::Peek()
{
	if (mIndex < mBuffer.size())
	{
		return mBuffer.at(mIndex);
	}
	throw StopCallInc("Buffer overflow at peek");
}

char Incantation::Lexer::PeekNext()
{
	if (mIndex + 1 < mBuffer.size())
	{
		return mBuffer.at(mIndex + 1);
	}
	throw StopCallInc("Buffer overflow at peek-next");
}

void Incantation::Lexer::Dump()
{
	if (mTemp.empty())
	{
		return;
	}
	if (mTemp.size() == 1) 
	{
		if (mTemp.front() == ' ')
		{
			mTemp.clear();
			return;
		}
		else if (mTemp.front() == '\n')
		{
			mTemp.clear();
			return;
		}
	}

	mRet.push_back(Token(mTemp, mLine));

	mTemp.clear();
}

void Incantation::Lexer::Add()
{
	mTemp.push_back(Peek());
}
