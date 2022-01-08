#include "Engine/Core/Main.h"
#include "Engine/Core/Windows.h"
#include "Engine/Utility/Error.h"

using namespace rv;

long to_icon(Severity severity)
{
	switch (severity)
	{
		case RV_SEVERITY_INFO:		return MB_ICONINFORMATION;
		case RV_SEVERITY_WARNING:	return MB_ICONWARNING;
		case RV_SEVERITY_ERROR:		return MB_ICONERROR;
		default:						return 0;
	}
}

void clean()
{
	resultHandler.Clear();
}

int main()
{

	try
	{
		resultHandler.RegisterResult(global_result);

		Result result = rv_main();

		if (result.fatal())
		{
			MessageBox(nullptr, ResultException(result).what(), "Result Exception", MB_OK | to_icon(result.severity()));
			clean();
			return EXIT_FAILURE;
		}
		else
		{
			clean();
			return EXIT_SUCCESS;
		}
	}
	catch (const ResultException& e)
	{
		MessageBox(nullptr, e.what(), "Result Exception", MB_OK | to_icon(e.result().severity()));
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...)
	{
		MessageBox(nullptr, "An unknown exception has occurred and the program has failed.\n\nPlease contact the developers", "Unknown Error", MB_OK | MB_ICONERROR);
	}
	clean();
	return EXIT_FAILURE;
}