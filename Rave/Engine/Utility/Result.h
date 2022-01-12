#pragma once
#include "Engine/Utility/Types.h"
#include "Engine/Utility/Hash.h"
#include "Engine/Utility/Flags.h"
#include "Engine/Utility/Identifier.h"
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

	static constexpr Identifier32 global_result = "Global Result";

	class Result
	{
	public:
		constexpr Result() : code((RV_SEVERITY_INFO & 0b111) | (global_result.hash() & ~0b111)) {}
		constexpr Result(Severity severity, const char* type) : code((severity & 0b111) | (hash<u32>(type) & ~0b111)) {}
		constexpr Result(Severity severity, const Identifier32& type) : code((severity & 0b111) | (type.hash() & ~0b111)) {}

		constexpr Severity severity() const { return static_cast<Severity>(code & 0b111); }
		constexpr u32 data() const { return code; }

		constexpr bool operator== (const Result& rhs) const { return code == rhs.code; }
		constexpr bool operator== (const Identifier32& type) const { return (code & ~0b111) == (type.hash() & ~0b111); }

		constexpr bool succeeded(Flags<Severity> success = make_flags<Severity>(RV_SEVERITY_INFO)) const { return success.contains(severity()); }
		constexpr bool failed(Flags<Severity> failure = make_flags<Severity>(RV_SEVERITY_WARNING, RV_SEVERITY_ERROR)) const { return failure.contains(severity()); }
		constexpr bool fatal() const { return severity() == RV_SEVERITY_ERROR; }

		void expect(const char* message);
		void expect(Flags<Severity> success = make_flags<Severity>(RV_SEVERITY_INFO));
		void expect(Flags<Severity> success, const char* message);
		void expect(Flags<Severity> success, const std::string& message);

	private:
		u32 code;
	};

	static constexpr Result success = Result(RV_SEVERITY_INFO, global_result);
	static constexpr Result warning = Result(RV_SEVERITY_WARNING, global_result);
	static constexpr Result failure = Result(RV_SEVERITY_ERROR, global_result);
}