#pragma once
#include <type_traits>
#include <xutility>
#include <array>
#include <string>
#include <string_view>
#include <vector>

namespace rv
{
	template<typename... Args>
	concept NonEmpty = sizeof...(Args) > 0;

	template<typename T>
	concept RangeType = requires(T range) 
	{
		range.begin();
		range.end();
	};

	template<typename T>
	concept CStringType = 
		std::is_same_v<char*, typename std::decay<T>::type> || 
		std::is_same_v<const char*, typename std::decay<T>::type> || 
		std::is_same_v<wchar_t*, typename std::decay<T>::type> || 
		std::is_same_v<const wchar_t*, typename std::decay<T>::type>;

	template<typename T>
	concept ArrayType = std::is_array_v<T> || std::is_same_v<T, std::array<typename T::value_type, T().size()>>;

	template<typename T>
	concept StdStringType = std::is_same_v<T, std::basic_string<typename T::value_type>> || std::is_same_v<T, std::basic_string_view<typename T::value_type>>;

	template<typename T>
	concept StdVectorType = std::is_same_v<T, std::vector<typename T::value_type>>;

	template<typename T>
	concept EnumType = std::is_enum_v<T>;

	template<typename T, typename D>
	concept ConvertibleType = std::is_convertible_v<T, D>;

	namespace detail
	{
		template<typename T, typename F, typename... Args>
		static constexpr bool all_convertible()
		{
			bool conv = ConvertibleType<F, T>;
			if constexpr (NonEmpty<Args...>)
				conv = conv && all_convertible<T, Args...>();
			return conv;
		}


		template<typename T, typename F, typename... Args>
		static constexpr bool all_convertible_and_arithmetic()
		{
			bool conv = ConvertibleType<F, T> && std::is_arithmetic_v<F>;
			if constexpr (NonEmpty<Args...>)
				conv = conv && all_convertible_and_arithmetic<T, Args...>();
			return conv;
		}
	}
}