#pragma once
#include "Engine/Utility/Concepts.h"
#include <sstream>

namespace rv
{
	namespace detail
	{
		template<typename C = char, typename F, typename... Args>
		void str(std::basic_ostringstream<C>& oss, const F& first, const Args&... args)
		{
			oss << first;
			if constexpr (NonEmpty<Args...>)
				str(oss, args...);
		}
	}

	template<typename C = char, typename... Args>
	std::basic_string<C> str(const Args&... args)
	{
		if constexpr (NonEmpty<Args...>)
		{
			std::basic_ostringstream<C> oss;
			detail::str(oss, args...);
			return oss.str();
		}
		return {};
	}
}