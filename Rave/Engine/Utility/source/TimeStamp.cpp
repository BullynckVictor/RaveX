#include "Engine/Utility/TimeStamp.h"

rv::TimeStamp::TimeStamp(const TimeZone& zone)
	:
	ymd(),
	hms()
{
	Reset(zone);
}
int rv::TimeStamp::year() const
{
	return (int)ymd.year();
}

unsigned int rv::TimeStamp::month() const
{
	return (unsigned int)ymd.month();
}

unsigned int rv::TimeStamp::day() const
{
	return (unsigned int)ymd.day();
}

int rv::TimeStamp::hours() const
{
	return hms.hours().count();
}

int rv::TimeStamp::minutes() const
{
	return hms.minutes().count();
}

int rv::TimeStamp::seconds() const
{
	return hms.seconds().count();
}

int rv::TimeStamp::milliseconds() const
{
	return hms.subseconds().count();
}

void rv::TimeStamp::Reset(const TimeZone& zone)
{
	auto tp = std::chrono::zoned_time( zone.zone(), std::chrono::system_clock::now()).get_local_time();
	auto dp = std::chrono::floor<std::chrono::days>(tp);
	ymd = std::chrono::year_month_day(dp);
	hms = std::chrono::hh_mm_ss(floor<std::chrono::milliseconds>(tp - dp));
}

rv::TimeZone::TimeZone()
	:
	m_zone(std::chrono::current_zone())
{
}

rv::TimeZone::TimeZone(std::string_view zone)
	:
	m_zone(std::chrono::locate_zone(zone))
{
}

std::string_view rv::TimeZone::name() const
{
	return m_zone->name();
}

const std::chrono::time_zone* rv::TimeZone::zone() const
{
	return m_zone;
}
