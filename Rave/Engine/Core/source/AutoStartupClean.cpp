#include "Engine/Core/AutoStartupClean.h"
#include "Engine/Utility/ResultHandler.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Error.h"

rv::Result rv::startup()
{
	resultHandler.RegisterResult(global_result);
	resultHandler.RegisterResult(condition_result);
	resultHandler.RegisterResult(assertion_result);
	resultHandler.RegisterResult(file_result);

	return success;
}

void rv::cleanup()
{
	resultHandler.Clear();
}

rv::AutoStartupClean::AutoStartupClean()
{
	startup().expect(utf16_string(strvalid(u"Startup routine failed")));
}

rv::AutoStartupClean::~AutoStartupClean()
{
	cleanup();
}
