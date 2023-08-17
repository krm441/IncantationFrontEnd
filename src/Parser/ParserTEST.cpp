#include "Core/pchInc.h"
#include "ParserTEST.h"
#include "Lexer/Lexer.h"
#include "Parser/RDP.h"

using namespace Incantation;

void TEST_RDP()
{
	Console console;
	try
	{
		std::fstream stream("ass_dir/mock.inc");

		if (!stream.is_open())
			throw "unable to open file";
		
		std::stringstream prog;
		prog << stream.rdbuf();

		Lexer lexer(prog.str());
		auto& ret = lexer.LexAll();

		console.log(ret);
		console.nl();

		RDP rdp(ret);
		rdp.Parse();
		
		rdp.Verbose();
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
	catch (const char* e)
	{
		console.log(e);
	}
	catch (...)
	{
		console.log("unknown exception");
	}
}
