#include "Engine/Utility/Result.h"
#include "Engine/Utility/ResultHandler.h"

void rv::Result::expect(utf16_string&& message) const
{
	expect(RV_SEVERITY_INFO, std::move(message));
}

void rv::Result::expect(Flags<Severity> success) const
{
	if (!succeeded(success))
		throw ResultException(*this);
}

void rv::Result::expect(Flags<Severity> success, utf16_string&& message) const
{
	if (!succeeded(success))
		throw ResultException(*this, std::move(message));
}

void rv::Result::expect(const utf16_string& message) const
{
	expect(RV_SEVERITY_INFO, message);
}

void rv::Result::expect(Flags<Severity> success, const utf16_string& message) const
{
	if (!succeeded(success))
		throw ResultException(*this, message);
}
