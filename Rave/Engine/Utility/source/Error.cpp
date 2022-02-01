#include "Engine/Utility/Error.h"
#include "Engine/Utility/ResultHandler.h"
#include "Engine/Utility/String.h"
#include "Engine/Utility/File.h"
#include <comdef.h>

rv::ErrorInfo::ErrorInfo(const char* source, uint64 line)
	:
	source(source),
	line(line)
{
}

rv::utf16_string rv::ErrorInfo::Describe() const
{
	return str16(
		u"File:\t\t", RelativeToSolution(source).c_str(), u'\n',
		u"Line:\t\t", line, u"\nDescription:\t"
	);
}

rv::ConditionInfo::ConditionInfo(bool condition, const char* name, const char* source, uint64 line)
	:
	ErrorInfo(source, line),
	condition(condition),
	name(name)
{
}

rv::utf16_string rv::ConditionInfo::Describe() const
{
	return str16(
		ErrorInfo::Describe(),
		u"Condition \"", name, condition ? u"\" succeeded" : u"\" failed"
	);
}

rv::FileInfo::FileInfo(const std::filesystem::path& file, const char* source, uint64 line)
	:
	ErrorInfo(source, line),
	file(file)
{
}

rv::FileInfo::FileInfo(std::filesystem::path&& file, const char* source, uint64 line)
	:
	ErrorInfo(source, line),
	file(std::move(file))
{
}

rv::utf16_string rv::FileInfo::Describe() const
{
	return str16(
		ErrorInfo::Describe(),
		u"Failed to open file ", file
	);
}

rv::HrInfo::HrInfo(HRESULT result, const char* source, uint64 line)
	:
	ErrorInfo(source, line),
	result(result)
{
}

rv::utf16_string rv::HrInfo::Describe() const
{
	return str16(
		ErrorInfo::Describe(),
		_com_error(result).ErrorMessage(), u'\n',
		u"Result:\t\t", std::hex, u"0x", result
	);
}

rv::Result rv::check_condition(bool condition, const char* name, const char* source, uint64 line)
{
	return check_condition(condition, name, source, line, {});
}

rv::Result rv::check_condition(bool condition, const char* name, const char* source, uint64 line, utf16_string&& message)
{
	if (condition)
		return succeeded_condition;

	if constexpr (resultHandler.enabled)
		resultHandler.PushResult(failed_condition, std::move(message), ConditionInfo(condition, name, source, line));

	return failed_condition;
}

rv::Result rv::check_assertion(bool assertion, const char* name, const char* source, uint64 line)
{
	return check_assertion(assertion, name, source, line, {});
}

rv::Result rv::check_assertion(bool assertion, const char* name, const char* source, uint64 line, utf16_string&& message)
{
	if constexpr (build.debug)
	{
		if (assertion)
			return succeeded_assertion;

		if constexpr (resultHandler.enabled)
			resultHandler.PushResult(failed_assertion, std::move(message), ConditionInfo(assertion, name, source, line));

		return failed_assertion;
	}
	else
		return succeeded_assertion;
}

rv::Result rv::check_file(const std::filesystem::path& path, const char* source, uint64 line)
{

	return check_file(path, source, line, {});
}

rv::Result rv::check_file(const std::filesystem::path& path, const char* source, uint64 line, utf16_string&& message)
{
	if (FileExists(path))
		return succeeded_file;

	if constexpr (resultHandler.enabled)
		resultHandler.PushResult(failed_file, std::move(message), FileInfo(path, source, line));

	return failed_file;
}

rv::Result rv::check_hr(HRESULT hr, const char* source, uint64 line)
{
	return check_hr(hr, source, line, {});
}

rv::Result rv::check_hr(HRESULT hr, const char* source, uint64 line, utf16_string&& message)
{
	if (SUCCEEDED(hr))
		return succeeded_hr;

	if constexpr (resultHandler.enabled)
		resultHandler.PushResult(failed_hr, std::move(message), HrInfo(hr, source, line));

	return failed_hr;
}

rv::Result rv::check_last(bool condition, const char* source, uint64 line)
{
	return check_hr((HRESULT)GetLastError(), source, line);
}

rv::Result rv::check_last(bool condition, const char* source, uint64 line, utf16_string&& message)
{
	return check_hr((HRESULT)GetLastError(), source, line, std::move(message));
}
