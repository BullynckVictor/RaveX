#pragma once
#include "Engine/Utility/Result.h"
#include <exception>
#include <sstream>
#include <map>
#include <mutex>

namespace rv
{
	class ResultHandler
	{
	public:
		void RegisterResult(const Identifier32& result);
		const char* GetResultName(const Result& result);

		void Clear();

	private:
		std::map<u32, const char*> nameMap;
		std::mutex nameMutex;
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