#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Queue.h"
#include "Engine/Core/Build.h"
#include <exception>
#include <sstream>
#include <map>
#include <mutex>

#ifndef RV_LOG_RESULTS
#if defined(RV_DEBUG) and not defined(RV_NO_LOG_RESULTS)
#define RV_LOG_RESULTS
#endif
#endif

namespace rv
{
	template<typename I>
	concept ResultInfoType = requires(I info) { info.Describe(); };

	class ResultQueue
	{
	public:
		ResultQueue() = default;
		ResultQueue(const ResultQueue&) = delete;
		ResultQueue(ResultQueue&& rhs) noexcept;

		ResultQueue& operator= (const ResultQueue&) = delete;
		ResultQueue& operator= (ResultQueue&& rhs) noexcept;

		struct ResultInfo
		{
			ResultInfo() = default;
			Result result;
			std::string message;
			std::string info;
		};

		template<typename I>
		void PushResult(Result result, std::string&& message, const I& info)
		{
			ResultInfo r;
			r.message = std::move(message);
			r.result = result;
			if constexpr (ResultInfoType<I>)
				r.info = info.Describe();
			queue.PushEntry(std::move(r), info);
		}
		template<typename I>
		void PushResult(Result result, std::string&& message, I&& info)
		{
			ResultInfo r;
			r.message = std::move(message);
			r.result = result;
			if constexpr (ResultInfoType<I>)
				r.info = info.Describe();
			queue.PushEntry(std::move(r), std::move(info));
		}

		void PushResult(Result result, std::string&& message);

		Queue<ResultInfo>::Header* GetResult(Flags<Severity> severity = RV_SEVERITY_ALL);

	private:
		Queue<ResultInfo> queue;

		friend struct ResultInfo;
	};

	struct ResultInfo
	{
	public:
		ResultInfo() = default;
		ResultInfo(Queue<ResultQueue::ResultInfo>::Header* header);
		ResultInfo(const ResultInfo&) = delete;
		ResultInfo(ResultInfo&& rhs) noexcept;
		~ResultInfo();

		ResultInfo& operator= (const ResultInfo&) = delete;
		ResultInfo& operator= (ResultInfo&& rhs) noexcept;

		operator bool() const;

		bool valid() const;
		bool invalid() const;

		Result& result();
		const Result& result() const;

		std::string& message();
		const std::string& message() const;

		std::string& description();
		const std::string& description() const;

		template<typename I>
		bool is_type() const { return header ? (header->type == typeid(I).hash_code()) : false; }

		template<typename I>
		I& info() { return *header->data<I>(); }
		template<typename I>
		const I& info() const { return *header->data<I>(); }

	private:
		Queue<ResultQueue::ResultInfo>::Header* header = nullptr;
	};

	class ResultHandler
	{
	public:
		void RegisterResult(const Identifier32& result);
		const char* GetResultName(const Result& result);

		ResultQueue& GetThreadQueue();
		std::vector<std::reference_wrapper<std::pair<const std::thread::id, ResultQueue>>> GetQueues();

		void Clear();

#		ifdef RV_LOG_RESULTS
		static constexpr bool enabled = true;
#		else
		static constexpr bool enabled = false;
#		endif

	private:
		std::map<u32, const char*> nameMap;
		std::mutex nameMutex;

		std::map<std::thread::id, ResultQueue> queueMap;
		std::mutex queueMutex;
	};

	class ResultException : public std::exception
	{
	public:
		ResultException() = default;
		ResultException(const Result& result);
		ResultException(const Result& result, const std::string& message);

		const char* what() const override;
		const Result& result() const;

	private:
		void Format(std::stringstream& ss);

		Result m_result;
		std::string m_message;
	};

	extern ResultHandler resultHandler;
}