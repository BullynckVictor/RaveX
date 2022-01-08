#pragma once
#include "Engine/Utility/Types.h"
#include "Engine/Utility/Concepts.h"

namespace rv
{
	template<typename T>
	static constexpr T make_flag(unsigned char bit) { return static_cast<T>(0x1 << bit); }

	namespace detail
	{
		struct FlagMaker;
	}

	template<EnumType E>
	struct Flags
	{
	public:
		constexpr Flags() : m_data(0) {}
		constexpr Flags(E flag) : m_data(flag) {}

		constexpr operator typename of_size<sizeof(E)>::type() const { return m_data; }
		constexpr typename of_size<sizeof(E)>::type data() const { return m_data; }

		constexpr Flags<E> operator|  (const Flags<E>& rhs) const { return m_data | rhs.m_data; }
		constexpr Flags<E> operator&  (const Flags<E>& rhs) const { return m_data & rhs.m_data; }
		constexpr Flags<E> operator^  (const Flags<E>& rhs) const { return m_data ^ rhs.m_data; }

		constexpr Flags<E>& operator|= (const Flags<E>& rhs) { m_data |= rhs.m_data; return *this; }
		constexpr Flags<E>& operator&= (const Flags<E>& rhs) { m_data &= rhs.m_data; return *this; }
		constexpr Flags<E>& operator^= (const Flags<E>& rhs) { m_data ^= rhs.m_data; return *this; }

		constexpr bool empty() const { return m_data == 0; }
		constexpr bool contain(const Flags& flags) const { return m_data & flags.m_data; }
		constexpr bool contains(const Flags& flags) const { return contain(flags); }
		constexpr bool get(const E& flags) const { return contain(flags); }

		constexpr Flags flipped() const { return ~m_data; }

		constexpr void flip() { m_data = ~m_data; }
		constexpr void set(Flags flags, bool value = true) { value ? (m_data ^= flags) : (m_data &= ~flags.m_data); }

	private:
		constexpr Flags(typename of_size<sizeof(E)>::type data) : m_data(data) {}

		typename of_size<sizeof(E)>::type m_data;
				
		friend struct detail::FlagMaker;
	};

	namespace detail
	{
		template<typename T, typename E>
		concept FlagType = EnumType<E> && (std::is_same_v<T, E> || std::is_same_v<T, Flags<E>>);

		template<EnumType E, FlagType<E> F, typename... Args>
		static constexpr void combine(typename of_size<sizeof(E)>::type& data, F flag, Args... flags)
		{
			data |= flag;
			if constexpr (NonEmpty<Args...>)
				combine<E>(data, flags...);
		}

		struct FlagMaker
		{
			template<EnumType E>
			static constexpr Flags<E> make_flags(typename of_size<sizeof(E)>::type data)
			{
				return data;
			}
		};
	}

	template<EnumType E, typename... Args>
	static constexpr Flags<E> make_flags(Args... flags)
	{
		typename of_size<sizeof(E)>::type data = 0;
		if constexpr (NonEmpty<Args...>)
			detail::combine<E>(data, flags...);
		return detail::FlagMaker::make_flags<E>(data);
	}
}