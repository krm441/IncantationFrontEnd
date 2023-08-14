#pragma once
#include <stdexcept>
#include <string>
#include <sstream>

namespace Incantation
{
	class StopCallInc : public std::exception
	{
	public:
		StopCallInc(const std::string& msg = "Inc Stop Call") : m_msg(msg)
		{

		}
		const char* what() const noexcept override
		{
			return m_msg.c_str();
		}

	private:
		std::string m_msg;
	};

	class SyntaxError : public std::exception
	{
	public:
		SyntaxError(const std::string& msg = "", int line = -1) : mMsg("Incantation::SyntaxError")
		{
			mMsg += ": " + msg;
			if (line > -1)
				mMsg += " , at line: \'" + std::to_string(line) + '\'';
		}
		template<class... Args>
		SyntaxError(Args... args) : mMsg("Incantation::SyntaxError")
		{
			std::stringstream stream;
			stream << mMsg << ' ';
			((stream << args << ' '), ...) << ' ';// << line;
			mMsg = stream.str();
		}
		const char* what() const noexcept override
		{
			return mMsg.c_str();
		}

	private:
		std::string mMsg;
	};

	struct SafeWhileLoop
	{
		SafeWhileLoop(int max_loops = 2000000) : mLoops(0), mLoopsMax(max_loops), mMsg() {}
		SafeWhileLoop(const std::string& msg, int max_loops = 2000000) : mLoops(0), mLoopsMax(max_loops), mMsg(msg) {}
		bool True() { 
			if (mLoops >= mLoopsMax && bRunning)
				throw StopCallInc("Potential infinite loop detected - stop call in module: " + mMsg);
			return mLoops++ < mLoopsMax;
		}
		void Stop() { bRunning = false; }
	private:
		int mLoops, mLoopsMax;
		std::string mMsg;
		bool bRunning = true;
	};
}