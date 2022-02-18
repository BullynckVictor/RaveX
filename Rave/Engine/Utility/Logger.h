#pragma once
#include "Engine/Utility/Queue.h"
#include "Engine/Utility/TimeStamp.h"
#include "Engine/Utility/Result.h"
#include "Engine/Core/Build.h"
#include "Engine/Utility/String.h"
#include <deque>
#include <mutex>

#if not defined(RV_DEBUG_LOGGER) and defined(RV_DEBUG) and not defined(RV_NO_DEBUG_LOGGER)
#define RV_DEBUG_LOGGER
#endif

namespace rv
{
	struct LogInfo
	{
		LogInfo() = default;

		Severity severity = RV_SEVERITY_NULL;
		TimeStamp stamp;
		utf16_string message;

		utf16_string Format() const;
		void Format(std::wostream& ss) const;
	};

	struct LogQueue
	{
		LogQueue() = default;

		Queue<LogInfo> queue;
		std::mutex mutex;
	};

	class Logger
	{
	public:
		Logger() = default;

		void Log(const utf16_string& message, Severity severity = RV_SEVERITY_INFO);

		template<typename I>
		void Log(const utf16_string& message, const I& data, Severity severity = RV_SEVERITY_INFO)
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
				listener->queue.PushEntry(info, data);
			}
		}

	protected:
		std::deque<LogInfo> loggedInfo;
		std::vector<std::shared_ptr<LogQueue>> listeners;
		std::mutex mutex;
		friend class LogListener;
	};

	struct LogEvent
	{
	public:
		LogEvent() = default;
		LogEvent(Queue<LogInfo>::Header* header);
		LogEvent(const LogEvent&) = delete;
		LogEvent(LogEvent&& rhs) noexcept;
		~LogEvent();

		LogEvent& operator= (const LogEvent&) = delete;
		LogEvent& operator= (LogEvent&& rhs) noexcept;

		operator bool() const;
		bool Empty() const;

		template<typename E>
		bool IsType() const { return header ? (header->type == typeid(E).hash_code()) : false; }

		template<typename E>
		E& Get() { return *header->data<E>(); }
		template<typename E>
		const E& Get() const { return *header->data<E>(); }

		TimeStamp& Time();
		const TimeStamp& Time() const;

		utf16_string& Message();
		const utf16_string& Message() const;

		Severity Severity() const;

		utf16_string Format() const;
		void Format(std::wostream& ss) const;

		void Clear();

	private:
		Queue<LogInfo>::Header* header = nullptr;
	};

	class LogListener
	{
	public:
		LogListener() = default;
		LogListener(Logger& logger);

		void Listen(Logger& logger);
		void StopListening();

		LogEvent GetEvent();

	private:
		std::shared_ptr<LogQueue> events;
	};

#	ifdef RV_DEBUG_LOGGER

	class DebugLogger : public Logger
	{
	public:
		DebugLogger() = default;

		void Log(const utf16_string& message, Severity severity = RV_SEVERITY_INFO);

		template<typename I>
		void Log(const utf16_string& message, const I& data, Severity severity = RV_SEVERITY_INFO)
		{
			WriteToSTD(message, severity);
			Logger::Log(message, data, severity);
		}

	private:
		void WriteToSTD(const utf16_string& message, Severity severity);
	};

#	else

	class DebugLogger
	{
	public:
		DebugLogger() = default;

		void Log(const utf16_string& message, Severity severity = RV_SEVERITY_INFO) {}

		template<typename I>
		void Log(const utf16_string& message, const I& data, Severity severity = RV_SEVERITY_INFO) {}
	};

#	endif

	extern DebugLogger debug;
}

#ifdef RV_DEBUG_LOGGER
#define rv_log(msg)					rv::debug.Log(msg)
#define rv_log_info(msg)			rv::debug.Log(msg, rv::RV_SEVERITY_INFO)
#define rv_log_warning(msg)			rv::debug.Log(msg, rv::RV_SEVERITY_WARNING)
#define rv_log_error(msg)			rv::debug.Log(msg, rv::RV_SEVERITY_ERROR)

#define rv_logstr(...)				rv::debug.Log(rv::str(__VA_ARGS__))
#define rv_logstr_info(...)			rv::debug.Log(rv::str(__VA_ARGS__), rv::RV_SEVERITY_INFO)
#define rv_logstr_warning(...)		rv::debug.Log(rv::str(__VA_ARGS__), rv::RV_SEVERITY_WARNING)
#define rv_logstr_error(...)		rv::debug.Log(rv::str(__VA_ARGS__), rv::RV_SEVERITY_ERROR)
#else
#define rv_log(msg)			
#define rv_log_info(msg)	
#define rv_log_warning(msg)	
#define rv_log_error(msg)	
#define rv_logstr(...)				
#define rv_logstr_info(...)			
#define rv_logstr_warning(...)		
#define rv_logstr_error(...)		

#endif