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

std::string rv::ErrorInfo::Describe() const
{
	return str(
		"File:\t", RelativeToSolution(source), '\n',
		"Line:\t", line, '\n'
	);
}

rv::ConditionInfo::ConditionInfo(bool condition, const char* name, const char* source, uint64 line)
	:
	ErrorInfo(source, line),
	condition(condition),
	name(name)
{
}

std::string rv::ConditionInfo::Describe() const
{
	return str(
		ErrorInfo::Describe(),
		"Condition \"", name, condition ? "\" succeeded\n" : "\" failed\n"
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

std::string rv::FileInfo::Describe() const
{
	return str(
		ErrorInfo::Describe(),
		"Failed to open file ", file, '\n'
	);
}

rv::HrInfo::HrInfo(HRESULT result, const char* source, uint64 line)
	:
	ErrorInfo(source, line),
	result(result)
{
}

std::string rv::HrInfo::Describe() const
{
	/*
	return str(
		ErrorInfo::Describe(),
		"Result:\t", std::hex, result, '\n',
		_com_error(result).Source().GetBSTR()
	);
	*/ return {};
}

rv::Result rv::check_condition(bool condition, const char* name, const char* source, uint64 line, const char* message)
{
	if (condition)
		return succeeded_condition;

	if constexpr (resultHandler.enabled)
		resultHandler.GetThreadQueue().PushResult(failed_condition, message, ConditionInfo(condition, name, source, line));
	
	return failed_condition;
}

rv::Result rv::check_condition(bool condition, const char* name, const char* source, uint64 line, std::string&& message)
{
	if (condition)
		return succeeded_condition;

	if constexpr (resultHandler.enabled)
		resultHandler.GetThreadQueue().PushResult(failed_condition, std::move(message), ConditionInfo(condition, name, source, line));

	return failed_condition;
}

rv::Result rv::check_assertion(bool assertion, const char* name, const char* source, uint64 line, const char* message)
{
	if constexpr (build.debug)
	{
		if (assertion)
			return succeeded_assertion;

		if constexpr (resultHandler.enabled)
			resultHandler.GetThreadQueue().PushResult(failed_assertion, message, ConditionInfo(assertion, name, source, line));

		return failed_assertion;
	}
	else
		return succeeded_assertion;
}

rv::Result rv::check_assertion(bool assertion, const char* name, const char* source, uint64 line, std::string&& message)
{
	if constexpr (build.debug)
	{
		if (assertion)
			return succeeded_assertion;

		if constexpr (resultHandler.enabled)
			resultHandler.GetThreadQueue().PushResult(failed_assertion, std::move(message), ConditionInfo(assertion, name, source, line));

		return failed_assertion;
	}
	else
		return succeeded_assertion;
}

rv::Result rv::check_file(const std::filesystem::path& path, const char* source, uint64 line, const char* message)
{
	if (FileExists(path))
		return succeeded_file;

	if constexpr (resultHandler.enabled)
		resultHandler.GetThreadQueue().PushResult(failed_file, message, FileInfo(path, source, line));

	return failed_file;
}

rv::Result rv::check_file(const std::filesystem::path& path, const char* source, uint64 line, std::string&& message)
{
	if (FileExists(path))
		return succeeded_file;

	if constexpr (resultHandler.enabled)
		resultHandler.GetThreadQueue().PushResult(failed_file, std::move(message), FileInfo(path, source, line));

	return failed_file;
}
