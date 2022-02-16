#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Utility/File.h"
#include "Engine/Core/Windows.h"
#include "Engine/Utility/String.h"
#include "Engine/Utility/ResultHandler.h"

namespace rv
{
	struct ErrorInfo
	{
		ErrorInfo() = default;
		ErrorInfo(const char* source, uint64 line);

		utf16_string Describe() const;
		
		const char* source = nullptr;
		uint64 line = 0;
	};

	struct ConditionInfo : public ErrorInfo
	{
		ConditionInfo() = default;
		ConditionInfo(bool condition, const char* name, const char* source, uint64 line);

		utf16_string Describe() const;

		bool condition;
		const char* name;
	};

	struct FileInfo : public ErrorInfo
	{
		FileInfo() = default;
		FileInfo(const std::filesystem::path& file, const char* source, uint64 line);
		FileInfo(std::filesystem::path&& file, const char* source, uint64 line);

		utf16_string Describe() const;

		std::filesystem::path file;
	};

	struct HrInfo : public ErrorInfo
	{
		HrInfo() = default;
		HrInfo(HRESULT result, const char* source, uint64 line);

		utf16_string Describe() const;

		HRESULT result;
	};

	static constexpr Identifier32 condition_result	= "Condition Result";
	static constexpr Identifier32 assertion_result	= "Assertion Result";
	static constexpr Identifier32 file_result		= "Assertion Result";
	static constexpr Identifier32 hr_result			= "HRESULT";


	static constexpr Result succeeded_condition		= Result(RV_SEVERITY_INFO, condition_result);
	static constexpr Result succeeded_assertion		= Result(RV_SEVERITY_INFO, condition_result);
	static constexpr Result succeeded_file			= Result(RV_SEVERITY_INFO, file_result);
	static constexpr Result succeeded_hr			= Result(RV_SEVERITY_INFO, hr_result);

	static constexpr Result failed_condition		= Result(RV_SEVERITY_ERROR, condition_result);
	static constexpr Result failed_assertion		= Result(RV_SEVERITY_ERROR, assertion_result);
	static constexpr Result failed_file				= Result(RV_SEVERITY_ERROR, file_result);
	static constexpr Result failed_hr				= Result(RV_SEVERITY_ERROR, hr_result);


	Result check_condition(bool condition, const char* name, const char* source, uint64 line);
	Result check_condition(bool condition, const char* name, const char* source, uint64 line, utf16_string&& message);

	Result check_assertion(bool assertion, const char* name, const char* source, uint64 line);
	Result check_assertion(bool assertion, const char* name, const char* source, uint64 line, utf16_string&& message);

	Result check_file(const std::filesystem::path& path, const char* source, uint64 line);
	Result check_file(const std::filesystem::path& path, const char* source, uint64 line, utf16_string&& message);

	Result check_hr(HRESULT hr, const char* source, uint64 line);
	Result check_hr(HRESULT hr, const char* source, uint64 line, utf16_string&& message);

	Result check_last(bool condition, const char* source, uint64 line);
	Result check_last(bool condition, const char* source, uint64 line, utf16_string&& message);
}

#define rv_result							rv::Result result = rv::success

#define rv_check_condition(condition)			rv::check_condition(condition, #condition, __FILE__, __LINE__)
#define rv_check_file(file)						rv::check_file(file, __FILE__, __LINE__)
#define rv_check_hr(hr)							rv::check_hr(hr, __FILE__, __LINE__)
#define rv_check_last(condition)				rv::check_last(condition, __FILE__, __LINE__)

#ifdef RV_LOG_RESULTS
#define rv_check_condition_msg(condition, msg)	rv::check_condition(condition, #condition, __FILE__, __LINE__, msg)
#define rv_check_file_msg(file, msg)			rv::check_file(file, __FILE__, __LINE__, msg)
#define rv_check_hr_msg(hr, msg)				rv::check_hr(hr, __FILE__, __LINE__, msg)
#define rv_check_last_msg(condition, msg)		rv::check_last(condition, __FILE__, __LINE__, msg)
#else
#define rv_check_condition_msg(condition, msg)	rv::check_condition(condition, #condition, __FILE__, __LINE__)
#define rv_check_file_msg(file, msg)			rv::check_file(file, __FILE__, __LINE__)
#define rv_check_hr_msg(hr, msg)				rv::check_hr(hr, __FILE__, __LINE__)
#define rv_check_last_msg(condition, msg)		rv::check_last(condition, __FILE__, __LINE__)
#endif

#ifdef RV_DEBUG
#define rv_assert(assertion)				rv::check_assertion(assertion, #assertion, __FILE__, __LINE__)
#ifdef RV_LOG_RESULTS
#define rv_assert_msg(assertion, msg)		rv::check_assertion(assertion, #assertion, __FILE__, __LINE__, msg)
#else
#define rv_assert_msg(assertion, msg)		rv::check_assertion(assertion, #assertion, __FILE__, __LINE__)
#endif
#else
#define rv_assert(assertion)
#define rv_assert_msg(assertion)
#endif

#define rv_rif(res)								if ((result = (res)).failed()) return result;
#define rif_check_condition(condition)			rv_rif(rv_check_condition(condition))
#define rif_assert(condition)					rv_rif(rv_assert(condition))
#define rif_check_file(file)					rv_rif(rv_check_file(file))
#define rif_check_hr(hr)						rv_rif(rv_check_hr(hr))
#define rif_check_last(condition)				rv_rif(rv_check_last(condition))
#define rif_check_condition_msg(condition, msg)	rv_rif(rv_check_condition_msg(condition, msg))
#define rif_assert_msg(condition, msg)			rv_rif(rv_assert_msg(condition, msg))
#define rif_check_file_msg(file, msg)			rv_rif(rv_check_file_msg(file, msg))
#define rif_check_hr_msg(hr, msg)				rv_rif(rv_check_hr_msg(hr, msg))
#define rif_check_last_msg(condition, msg)		rv_rif(rv_check_last_msg(condition, msg))