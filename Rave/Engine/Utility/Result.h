#pragma once
#include "Engine/Utility/Types.h"
#include "Engine/Utility/Hash.h"
#include "Engine/Utility/Flags.h"
#include "Engine/Utility/Identifier.h"
#include "Engine/Utility/String.h"
#include <memory>

namespace rv
{
	enum Severity
	{
		RV_SEVERITY_NULL	= 0,
		RV_SEVERITY_INFO	= make_flag<Severity>(0),
		RV_SEVERITY_WARNING = make_flag<Severity>(1),
		RV_SEVERITY_ERROR	= make_flag<Severity>(2),
		RV_SEVERITY_ALL		= RV_SEVERITY_INFO | RV_SEVERITY_WARNING | RV_SEVERITY_ERROR,
	};
	
	static constexpr const char* to_string(Severity severity)
	{
		switch (severity)
		{
			case RV_SEVERITY_NULL:		return "Null";
			case RV_SEVERITY_INFO:		return "Info";
			case RV_SEVERITY_WARNING:	return "Warning";
			case RV_SEVERITY_ERROR:		return "Error";
			case RV_SEVERITY_ALL:		return "Info - Warning - Error";
			default:					return nullptr;
		}
	}
	static constexpr const wchar_t* to_wstring(Severity severity)
	{
		switch (severity)
		{
			case RV_SEVERITY_NULL:		return L"Null";
			case RV_SEVERITY_INFO:		return L"Info";
			case RV_SEVERITY_WARNING:	return L"Warning";
			case RV_SEVERITY_ERROR:		return L"Error";
			case RV_SEVERITY_ALL:		return L"Info - Warning - Error";
			default:					return nullptr;
		}
	}

	static constexpr Identifier32 global_result = "Global Result";

	class Result
	{
	public:
		constexpr Result() : code(global_result.hash()), sev(RV_SEVERITY_INFO) {}
		constexpr Result(Severity severity, const char* type) : code(rv::hash<u32>(type)), sev(severity) {}
		constexpr Result(Severity severity, const Identifier32& type) : code(type.hash()), sev(severity) {}

		constexpr Severity severity() const { return sev; }
		constexpr u32 data() const { return code; }
		constexpr u32 hash() const { return code; }

		constexpr bool operator== (const Result& rhs) const { return code == rhs.code && sev == rhs.sev; }
		constexpr bool operator== (const Identifier32& type) const { return code == type.hash(); }

		constexpr bool succeeded(Flags<Severity> success = make_flags<Severity>(RV_SEVERITY_INFO)) const { return success.contains(severity()); }
		constexpr bool failed(Flags<Severity> failure = make_flags<Severity>(RV_SEVERITY_WARNING, RV_SEVERITY_ERROR)) const { return failure.contains(severity()); }
		constexpr bool fatal() const { return severity() == RV_SEVERITY_ERROR; }

		void expect(utf16_string&& message) const;
		void expect(Flags<Severity> success = make_flags<Severity>(RV_SEVERITY_INFO)) const;
		void expect(Flags<Severity> success, utf16_string&& message) const;
		void expect(const utf16_string& message) const;
		void expect(Flags<Severity> success, const utf16_string& message) const;

	private:
		u32 code;
		Severity sev;
	};

	static constexpr Result success = Result(RV_SEVERITY_INFO, global_result);
	static constexpr Result warning = Result(RV_SEVERITY_WARNING, global_result);
	static constexpr Result failure = Result(RV_SEVERITY_ERROR, global_result);
}