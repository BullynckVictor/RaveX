#pragma once
#include <limits>
#include <type_traits>

namespace rv
{
	template<typename T>
	struct DefaultInvalidValue
	{
		static constexpr T get_invalid()
		{
			if constexpr (std::is_floating_point_v<T>)
				return std::numeric_limits<T>::quiet_NaN();
			if constexpr (std::is_pointer_v<T>)
				return nullptr;
			if constexpr (std::is_integral_v<T>)
				if constexpr (std::is_signed_v<T>)
					return std::numeric_limits<T>::min();
				else
					return std::numeric_limits<T>::max();
			return {};
		}
		static constexpr T value = get_invalid();
	};

	template<typename T, typename I = DefaultInvalidValue<T>>
	struct Optional
	{
		static constexpr T invalid_value = I::value;

		constexpr Optional() : value(invalid_value) {}
		constexpr Optional(const T& value) : value(value) {}
		constexpr Optional(T&& value) : value(std::move(value)) {}

		constexpr bool valid()		const { return value != invalid_value; }
		constexpr bool invalid()	const { return value == invalid_value; }
		constexpr operator bool()	const { return valid(); }

		T value;
	};
}