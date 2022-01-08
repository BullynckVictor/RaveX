#pragma once
#include <chrono>

namespace rv
{
	using namespace std::chrono_literals;

	class TimeZone
	{
	public:
		TimeZone();
		TimeZone(std::string_view zone);

		std::string_view name() const;
		const std::chrono::time_zone* zone() const;

	private:
		const std::chrono::time_zone* m_zone;
	};



	class TimeStamp
	{
	public:
		TimeStamp(const TimeZone& zone = TimeZone());

				 int year() const;
		unsigned int month() const;
		unsigned int day() const;
				 int hours() const;
				 int minutes() const;
				 int seconds() const;
				 int milliseconds() const;

		void Reset(const TimeZone& zone = TimeZone());

	private:
		std::chrono::year_month_day ymd;
		std::chrono::hh_mm_ss<std::chrono::milliseconds> hms;
	};
}

#ifndef RV_NO_CHRONO_LITERALS
using namespace std::chrono_literals;
#endif