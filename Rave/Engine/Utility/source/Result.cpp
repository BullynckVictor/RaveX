#include "Engine/Utility/Result.h"
#include "Engine/Utility/Error.h"

void rv::Result::expect(const char* message)
{
	expect(RV_SEVERITY_INFO, message);
}

void rv::Result::expect(Flags<Severity> success)
{
	
}

void rv::Result::expect(Flags<Severity> success, const char* message)
{
}
