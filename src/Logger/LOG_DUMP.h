#pragma once
#include <iostream>
#include <fstream>
#include "LOG.h"

namespace Incantation
{
	/// <summary>
	/// For debug
	/// </summary>
	class LOG_DUMP
	{
	public:
		~LOG_DUMP()
		{
			std::ofstream outa("ass_dir/LOG.log", std::ios::trunc);			
			std::stringstream dump;
			dump << "\n\nLOG::Incantation Parsing Begin\n";
			dump << out.str();
			if(mExit)
				std::cout << dump.str();
			outa << dump.rdbuf();
		}
		template<class... Args>
		inline void log(Args... args)
		{
			((out << args << " "), ...) << std::endl;
		}
		void DumpOnExit(bool i)
		{
			mExit = i;
		}
	private:
		std::stringstream out;
		bool mExit = false;
	};

//#define DUMPLOG
#ifdef DUMPLOG
	inline LOG_DUMP dumplog;
#define DUMP(...) \
	dumplog.log(__VA_ARGS__); \
	console.log(__VA_ARGS__);
#endif
}