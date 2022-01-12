#pragma once

#ifdef NDEBUG
#define RV_RELEASE
#else
#define RV_DEBUG
#endif

#ifdef WIN32
#define RV_x86
#else
#define RV_x64
#endif

namespace rv
{
	static constexpr struct BuildInfo
	{
#		ifdef RV_DEBUG
		static constexpr bool debug = true;
		static constexpr bool release = false;
#		else
		static constexpr bool debug = false;
		static constexpr bool release = true;
#		endif

#ifdef RV_x86
		static constexpr bool x86 = true;
		static constexpr bool x64 = false;
#else
		static constexpr bool x86 = false;
		static constexpr bool x64 = true;
#endif
	} build;
}