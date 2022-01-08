#pragma once
#include "Engine/Utility/Hash.h"

namespace rv
{
	template<HashType H>
	struct BasicIdentifier
	{
	public:
		BasicIdentifier() = default;
		constexpr BasicIdentifier(const char* name) : m_name(name), m_hash(rv::hash<H>(name)) {}

		constexpr operator H () const { return hash(); }

		constexpr H hash() const { return m_hash; };
		constexpr const char* name() const { return m_name; }

	private:
		H m_hash = std::numeric_limits<H>::max();
		const char* m_name = nullptr;
	};

	typedef BasicIdentifier<size_t> Identifier;
	typedef BasicIdentifier<u32> Identifier32;
	typedef BasicIdentifier<u64> Identifier64;
}