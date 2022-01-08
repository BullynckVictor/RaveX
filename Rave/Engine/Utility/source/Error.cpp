#include "Engine/Utility/Error.h"

rv::ResultHandler rv::resultHandler;

rv::ResultException::ResultException(const Result& result)
	:
	m_result(result)
{
	std::stringstream ss;
	Format(ss);
	m_message = ss.str();
}

rv::ResultException::ResultException(const Result& result, const std::string& message)
	:
	m_result(result)
{
	std::stringstream ss;
	Format(ss);
	ss << "\n\n" << "Message: " << message;
	m_message = ss.str();
}

const char* rv::ResultException::what() const
{
	return m_message.c_str();
}

const rv::Result& rv::ResultException::result() const
{
	return m_result;
}

void rv::ResultException::Format(std::stringstream& ss)
{
	const char* name = resultHandler.GetResultName(m_result);
	if (!name)
		name = "Unknown";

	ss << "Result Exception occurred!\n\n";
	ss << "Type: " << name << '\n';
	ss << "Severity: " << to_string(m_result.severity());
}

void rv::ResultHandler::RegisterResult(const Identifier32& result)
{
	std::lock_guard guard(nameMutex);
	nameMap[result.hash() & ~0b111] = result.name();
}

const char* rv::ResultHandler::GetResultName(const Result& result)
{
	std::lock_guard guard(nameMutex);
	u32 key = result.data() & ~0b111;
	return nameMap[key];
}

void rv::ResultHandler::Clear()
{
	std::lock_guard guard(nameMutex);
	nameMap.clear();
}
