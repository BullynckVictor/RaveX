#include "Engine/Utility/Logger.h"

rv::DebugLogger rv::debug;

void rv::Logger::Log(const utf16_string& message, Severity severity)
{
	LogInfo info;
	info.message = message;
	info.severity = severity;

	std::lock_guard global_guard(mutex);
	loggedInfo.push_back(info);
	for (auto& listener : listeners)
	{
		if (listener.use_count() == 1)
		{
			listener.reset();
			continue;
		}
		std::lock_guard guard(listener->mutex);
		listener->queue.PushEntry(info);
	}
}

rv::LogEvent::LogEvent(Queue<LogInfo>::Header* header)
	:
	header(header)
{
	if (header)
	{
		header->prev = nullptr;
		header->next = nullptr;
	}
}

rv::LogEvent::LogEvent(LogEvent&& rhs) noexcept
	:
	header(header)
{
	rhs.header = nullptr;
}

rv::LogEvent::~LogEvent()
{
	Clear();
}

rv::LogEvent& rv::LogEvent::operator=(LogEvent&& rhs) noexcept
{
	Clear();
	header = rhs.header;
	rhs.header = nullptr;
	return *this;
}

rv::LogEvent::operator bool() const
{
	return !Empty();
}

bool rv::LogEvent::Empty() const
{
	return !header;
}

rv::TimeStamp& rv::LogEvent::Time()
{
	return header->info.stamp;
}

const rv::TimeStamp& rv::LogEvent::Time() const
{
	return header->info.stamp;
}

rv::utf16_string& rv::LogEvent::Message()
{
	return header->info.message;
}

const rv::utf16_string& rv::LogEvent::Message() const
{
	return header->info.message;
}

rv::Severity rv::LogEvent::Severity() const
{
	return header->info.severity;
}

rv::utf16_string rv::LogEvent::Format() const
{
	return header->info.Format();
}

void rv::LogEvent::Format(std::wostream& ss) const
{
	header->info.Format(ss);
}

void rv::LogEvent::Clear()
{
	if (header)
		delete header;
}

rv::LogListener::LogListener(Logger& logger)
{
	Listen(logger);
}

void rv::LogListener::Listen(Logger& logger)
{
	events = std::make_shared<LogQueue>();
	std::lock_guard guard(logger.mutex);
	logger.listeners.push_back(events);
}

void rv::LogListener::StopListening()
{
	events.reset();
}

rv::LogEvent rv::LogListener::GetEvent()
{
	if (!events)
		return nullptr;
	std::lock_guard guard(events->mutex);
	return events->queue.GetHeader();
}

#ifdef RV_DEBUG_LOGGER

#include <iostream>

void rv::DebugLogger::Log(const utf16_string& message, Severity severity)
{
	WriteToSTD(message, severity);
	Logger::Log(message, severity);
}

void rv::DebugLogger::WriteToSTD(const utf16_string& message, Severity severity)
{
	std::lock_guard guard(mutex);
	LogInfo info;
	info.message = message;
	info.severity = severity;
	info.Format(std::wcout);
	std::cout << '\n';
}

#endif

rv::utf16_string rv::LogInfo::Format() const
{
	std::wostringstream ss;
	Format(ss);
	return ss.str();
}

void rv::LogInfo::Format(std::wostream& ss) const
{
	ss << L'[';
	if (stamp.hours() < 10)
		ss << L'0' << stamp.hours() << L':';
	else
		ss << stamp.hours() << L':';
	if (stamp.minutes() < 10)
		ss << L'0' << stamp.minutes() << L':';
	else
		ss << stamp.minutes() << L':';
	if (stamp.seconds() < 10)
		ss << L'0' << stamp.seconds() << L"]\t";
	else
		ss << stamp.seconds() << L"]\t";

	switch (severity)
	{
		case RV_SEVERITY_NULL:		ss << L"<NULL>     "; break;
		case RV_SEVERITY_INFO:		ss << L"<INFO>     "; break;
		case RV_SEVERITY_WARNING:	ss << L"<WARNING>  "; break;
		case RV_SEVERITY_ERROR:
		case RV_SEVERITY_ALL:		ss << L"<ERROR>    "; break;
	}

	ss << message.c_str<wchar_t>();
}