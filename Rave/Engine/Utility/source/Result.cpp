#include "Engine/Utility/Result.h"
#include "Engine/Utility/Error.h"

void rv::Result::expect(const char* message)
{
	expect(RV_SEVERITY_INFO, message);
}

void rv::Result::expect(Flags<Severity> success)
{
	if (!succeeded(success))
		throw ResultException(*this);
}

void rv::Result::expect(Flags<Severity> success, const char* message)
{
	if (!succeeded(success))
		throw ResultException(*this, message);
}

void rv::Result::expect(Flags<Severity> success, const std::string& message)
{
	if (!succeeded(success))
		throw ResultException(*this, message);
}
