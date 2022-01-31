#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Utility/File.h"
#include "Engine/Core/Windows.h"

namespace rv
{
	struct ErrorInfo
	{
		ErrorInfo() = default;
		ErrorInfo(const char* source, uint64 line);

		std::string Describe() const;
		
		const char* source = nullptr;
		uint64 line = 0;
	};

	struct ConditionInfo : public ErrorInfo
	{
		ConditionInfo() = default;
		ConditionInfo(bool condition, const char* name, const char* source, uint64 line);

		std::string Describe() const;

		bool condition;
		const char* name;
	};

	struct FileInfo : public ErrorInfo
	{
		FileInfo() = default;
		FileInfo(const std::filesystem::path& file, const char* source, uint64 line);
		FileInfo(std::filesystem::path&& file, const char* source, uint64 line);

		std::string Describe() const;

		std::filesystem::path file;
	};

	struct HrInfo : public ErrorInfo
	{
		HrInfo() = default;
		HrInfo(HRESULT result, const char* source, uint64 line);

		std::string Describe() const;

		HRESULT result;
	};

	static constexpr Identifier32 condition_result	= "Condition Result";
	static constexpr Identifier32 assertion_result	= "Assertion Result";
	static constexpr Identifier32 file_result		= "Assertion Result";


	static constexpr Result succeeded_condition		= Result(RV_SEVERITY_INFO, condition_result);
	static constexpr Result succeeded_assertion		= Result(RV_SEVERITY_INFO, condition_result);
	static constexpr Result succeeded_file			= Result(RV_SEVERITY_INFO, file_result);

	static constexpr Result failed_condition		= Result(RV_SEVERITY_ERROR, condition_result);
	static constexpr Result failed_assertion		= Result(RV_SEVERITY_ERROR, assertion_result);
	static constexpr Result failed_file				= Result(RV_SEVERITY_ERROR, file_result);


	Result check_condition(bool condition, const char* name, const char* source, uint64 line, const char* message = nullptr);
	Result check_condition(bool condition, const char* name, const char* source, uint64 line, std::string&& message);

	Result check_assertion(bool assertion, const char* name, const char* source, uint64 line, const char* message = nullptr);
	Result check_assertion(bool assertion, const char* name, const char* source, uint64 line, std::string&& message);

	Result check_file(const std::filesystem::path& path, const char* source, uint64 line, const char* message = nullptr);
	Result check_file(const std::filesystem::path& path, const char* source, uint64 line, std::string&& message);

	Result check_hr(HRESULT hr, const char* source, uint64 line, const char* message = nullptr);
	Result check_hr(HRESULT hr, const char* source, uint64 line, std::string&& message);
}