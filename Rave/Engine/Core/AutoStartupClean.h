#pragma once
#include "Engine/Utility/Result.h"

namespace rv
{
	Result startup();
	void cleanup();

	struct AutoStartupClean
	{
		AutoStartupClean();
		~AutoStartupClean();
	};
}