#pragma once
#include "Engine/Utility/Types.h"
#include "Engine/Utility/Concepts.h"
#include <type_traits>
#include <string_view>

namespace rv
{
	template<typename H>
	concept HashType = std::is_unsigned_v<H> && sizeof(H) >= 4;

	template<typename H>
	static constexpr H combine_hash(const H& h1, const H& h2)
	{
		return h1 ^ (h2 + (H)0x9e3779b9 + (h1 << 6) + (h1 >> 2));
	}

	namespace detail
	{
		template<HashType H>
		struct HashInfo
		{
			static constexpr H prime = 0;
			static constexpr H basis = 0;
		};

		template<>
		struct HashInfo<u32>
		{
			static constexpr u32 prime = 0x01000193;
			static constexpr u32 basis = 0x811c9dc5;
		};

		template<>
		struct HashInfo<u64>
		{
			static constexpr u64 prime = 0x00000100000001b3;
			static constexpr u64 basis = 0xcbf29ce484222325;
		};

		template<size_t D, HashType H, typename T>
		static constexpr void fnv1a_shift(H& hash, const T& key)
		{
			hash = hash ^ ((key >> (D * 8)) & 0xFF);
			hash = hash * HashInfo<H>::prime;
			if constexpr (D + 1 < sizeof (T))
				fnv1a_shift<D + 1>(hash, key);
		}

		template<HashType H, typename T>
		static constexpr H fnv1a(const T& key)
		{
			H hash = HashInfo<H>::basis;
			fnv1a_shift<0>(hash, key);
			return hash;
		}

		template<HashType H, typename T>
		static constexpr H fnv1a_range(const T* key, size_t length)
		{
			H hash = HashInfo<H>::basis;
			for (const T* value = key; value < key + length; ++value)
				fnv1a_shift<0>(hash, *value);
			return hash;
		}

		template<HashType H, size_t D, size_t L, typename T>
		static constexpr void fnv1a_ct_range_element(H& hash, const T& key)
		{
			fnv1a_shift<0>(hash, key[D]);
			if constexpr (D + 1 < L)
				fnv1a_ct_range_element<H, D + 1, L>(hash, key);
		}

		template<HashType H, typename T>
		static constexpr H fnv1a_ct_range(const T& key)
		{
			H hash = HashInfo<H>::basis;
			fnv1a_ct_range_element<H, 0, sizeof(key) / sizeof(key[0])>(hash, key);
			return hash;
		}

		template<HashType H, size_t L, typename T>
		static constexpr H fnv1a_ct_l_range(const T& key)
		{
			H hash = HashInfo<H>::basis;
			fnv1a_ct_range_element<H, 0, L>(hash, key);
			return hash;
		}

		template<HashType H, typename T>
		static constexpr H fnv1a_string(const T* key)
		{
			H hash = HashInfo<H>::basis;
			for (const T* value = key; *value; ++value)
				fnv1a_shift<0>(hash, *value);
			return hash;
		}

		template<HashType H, typename T>
		static constexpr H hash_element(const T& value)
		{
			if constexpr (std::is_integral_v<T>)
				return fnv1a<H>(value);

			else if constexpr (ArrayType<T>)
				if constexpr (std::is_integral_v<std::decay<decltype(value[0])>::type>)
					return fnv1a_ct_range<H>(value);
				else if constexpr (!std::is_class_v<std::decay<decltype(value[0])>::type>)
					return fnv1a_ct_l_range<H, sizeof(T)>(reinterpret_cast<const byte*>(value));
				else
				{
					H h = HashInfo<H>::basis;
					for (const auto& element : value)
						h = combine_hash(h, hash<H>(element));
					return h;
				}

			else if constexpr (CStringType<T>)
				if (value)
					return fnv1a_string<H>(value);
				else
					return HashInfo<H>::basis;

			else if constexpr (StdStringType<T> || StdVectorType<T>)
				return fnv1a_range<H>(value.data(), value.size());

			else if constexpr (RangeType<T>)
			{
				H h = HashInfo<H>::basis;
				for (const auto& element : value)
					h = combine_hash(h, hash<H>(element));
				return h;
			}

			else
				return fnv1a_ct_l_range<H, sizeof(T)>(reinterpret_cast<const byte*>(&value));
		}

		template<HashType H, typename F, typename... Args>
		static constexpr H hash(const F& value, const Args&... args)
		{
			H h = hash_element<H>(value);
			if constexpr (NonEmpty<Args...>)
				h = combine_hash(h, hash<H>(args...));
			return h;
		}
	}

	template<HashType H = size_t, typename... Args>
	static constexpr H hash(const Args&... args)
	{
		if constexpr (NonEmpty<Args...>)
			return detail::hash<H>(args...);
		else
			return detail::HashInfo<H>::basis;
	}

	using cstring = std::string_view;
}