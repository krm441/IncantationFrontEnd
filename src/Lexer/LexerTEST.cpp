#include "Core/IncFrontPCH.h"
#include "LexerTEST.h"
#include "Logger/LOG.h"

using namespace Incantation;

void TEST_LEXER()
{
	Console console;
	try
	{
		std::fstream stream("ass_dir/mock.inc");

		std::stringstream prog;
		prog << stream.rdbuf();

		Lexer lexer(prog.str());
		auto ret = lexer.LexAll();

		console.log(ret);
	}
	catch (const StopCallInc& e)
	{
		console.log(e.what());
		(void)e;
	}
	catch (const std::exception& e)
	{
		console.log(e.what());
		(void)e;
	}
}
