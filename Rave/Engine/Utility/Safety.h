#pragma once
#include <type_traits>
#include <algorithm>
#include <limits>

namespace rv
{
	namespace detail
	{
		template<typename T1, typename T2>
		static constexpr T2 signed_cast(const T2& source)
		{
			if constexpr (std::is_signed_v<T2> && std::is_unsigned_v<T1>)
				return std::max(0, source);
			else
				return source;
		}

		template<typename T1, typename T2>
		static constexpr T2 bounds_cast(const T2& source)
		{
			if constexpr (sizeof(T2) > sizeof(T1))
				if constexpr (std::is_signed_v<T2>)
					return std::clamp(source, std::numeric_limits<T2>::min(), std::numeric_limits<T2>::max());
				else
					return std::min(source, std::numeric_limits<T2>::max());
			return source;
		}
	}

	template<typename T1, typename T2>
	static constexpr T1 safe_cast(const T2& source)
	{
		if constexpr (std::is_floating_point_v<T1>)
			return static_cast<T1>(source);
		else
			return static_cast<T1>(detail::bounds_cast<T1>(detail::signed_cast<T1, T2>(source)));
	}
}