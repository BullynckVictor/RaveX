#pragma once
#include "Engine/Utility/Concepts.h"
#include "Engine/Utility/Types.h"
#include "Engine/Core/Build.h"

namespace rv
{
	template<typename T>
	static constexpr T make_mask(u8 begin, u8 end)
	{
		if (end < begin || end >= sizeof(T) * 8)
			return 0;
		T mask = 0;
		for (u8 i = begin; i <= end; ++i)
			mask |= static_cast<T>(0x1) << i;
		return mask;
	}

	namespace detail
	{
		template<typename T, u8 B, u8 E>
		struct mask
		{
			static constexpr T compute() { if constexpr (B == E) return static_cast<T>(0x1) << B; else return mask<T, B + 1, E>::value | (static_cast<T>(0x1) << B); }
			static constexpr T value = compute();
		};

	}

	template<typename T, u8 B, u8 E>
	static constexpr T make_mask_ct()
	{
		if constexpr (E < B || E >= sizeof(T) * 8)
			return 0;
		else
			return detail::mask<T, B, E>::value;
	}

	struct CodePoint;

	namespace encoding
	{
		template<size_t S>
		struct char_size
		{
		};

		template<> struct char_size<1> { using type = char8_t; };
		template<> struct char_size<2> { using type = char16_t; };
		template<> struct char_size<4> { using type = char32_t; };

		static constexpr char32_t invalid_char = static_cast<char32_t>(-1);

		static constexpr char32_t code_point(char8_t c)
		{
			if (c >= 0)
				return static_cast<char32_t>(c);
			return invalid_char;
		}
		static constexpr char32_t code_point(char16_t c)
		{
			if (c < 0xD800 || c > 0xDFFF)
				return static_cast<char32_t>(c);
			return invalid_char;
		}
		static constexpr char32_t code_point(char32_t c)
		{
			return c;
		}
		static constexpr char32_t code_point(char c)
		{
			return code_point(static_cast<typename char_size<sizeof(c)>::type>(c));
		}
		static constexpr char32_t code_point(wchar_t c)
		{
			return code_point(static_cast<typename char_size<sizeof(c)>::type>(c));
		}
		static constexpr void set_unchecked(CodePoint& cp, char32_t c);

		template<typename C, typename E>
		concept SameCharacterType = sizeof(E) == sizeof(C);
		template<typename C>
		concept CharacterType =
			std::is_same_v<typename std::decay<C>::type, char8_t>	||
			std::is_same_v<typename std::decay<C>::type, char16_t>	||
			std::is_same_v<typename std::decay<C>::type, char32_t>	||
			std::is_same_v<typename std::decay<C>::type, char>		||
			std::is_same_v<typename std::decay<C>::type, wchar_t>;
	}

	struct CodePoint
	{
	public:
		constexpr CodePoint() : point(0) {}
		constexpr CodePoint(char c) : point(encoding::code_point(c)) {}
		constexpr CodePoint(wchar_t c) : point(encoding::code_point(c)) {}
		constexpr CodePoint(char8_t c) : point(encoding::code_point(c)) {}
		constexpr CodePoint(char16_t c) : point(encoding::code_point(c)) {}
		constexpr CodePoint(char32_t c) : point(encoding::code_point(c)) {}
		constexpr CodePoint(const CodePoint&) = default;
		constexpr CodePoint(CodePoint&&) noexcept = default;

		constexpr CodePoint& operator= (const CodePoint&) = default;
		constexpr CodePoint& operator= (CodePoint&&) = default;

		constexpr bool operator== (CodePoint rhs) const { return point == rhs.point; }
		constexpr bool operator!= (CodePoint rhs) const { return point != rhs.point; }
		constexpr bool operator<= (CodePoint rhs) const { return point <= rhs.point; }
		constexpr bool operator>= (CodePoint rhs) const { return point >= rhs.point; }
		constexpr bool operator<  (CodePoint rhs) const { return point < rhs.point; }
		constexpr bool operator>  (CodePoint rhs) const { return point > rhs.point; }

		constexpr char32_t character() const { return point; }
		constexpr operator char32_t()  const { return point; }

		constexpr bool valid() const { return point <= 0x10FFFF; }
		constexpr bool invalid() const { return point > 0x10FFFF; }

		constexpr void make_valid() { if (invalid()) point = U'\uFFFD'; }

		//constexpr void directionality() const {  }
		//constexpr bool valid() const { return point != encoding::invalid_char; }
		//constexpr bool invalid() const { return point == encoding::invalid_char; }
		//constexpr bool is_alphabetic() const { return false; }
		//constexpr bool is_digit() const { return false; }
		//constexpr bool is_letter() const { return false; }
		//constexpr bool is_lower() const { return false; }
		//constexpr bool is_upper() const { return false; }
		//constexpr bool is_whitespace() const { return false; }
		//constexpr u8 size() const { return 0; }

	private:
		char32_t point;

		friend constexpr void encoding::set_unchecked(CodePoint&, char32_t);
	};

	namespace encoding
	{
		static constexpr void set_unchecked(CodePoint& cp, char32_t c)
		{
			cp.point = c;
		}
		static constexpr CodePoint make_unchecked_code_point(char32_t c) { CodePoint point; set_unchecked(point, c); return point; }
	}
	static constexpr CodePoint invalid_code_point = encoding::make_unchecked_code_point(-1);

	namespace detail
	{
		template<typename C>
		static constexpr bool byte_string_equal(const C* a, const C* b)
		{
			if (a == b)
				return true;
			if (!a || !b)
				return false;
			while (true)
			{
				if (*a != *b)
					return false;
				if (*a == (C)'\0')
					return true;
				++a;
				++b;
			}
		}
		template<typename C>
		static constexpr bool byte_string_not_equal(const C* a, const C* b)
		{
			if (a == b)
				return false;
			if (!a || !b)
				return true;
			while (true)
			{
				if (*a == *b)
					return true;
				if (*a == '\0' || *b == '\0')
					return false;
				++a;
				++b;
			}
		}
	}

	namespace encoding
	{
		constexpr bool valid_header(const char8_t* string, u8 offset)
		{
			return (string[offset] & make_mask_ct<char8_t, 6, 7>()) == (0b10 << 6);
		}
	}

	struct null_iterator {};

	template<encoding::CharacterType C>
	struct encoded_iterator_base
	{
	public:
		constexpr encoded_iterator_base() : string(nullptr), point(invalid_code_point) {}
		template<encoding::SameCharacterType<C> C2> constexpr encoded_iterator_base(const C2* string) : string(reinterpret_cast<const C*>(string)), point(invalid_code_point) {}
		template<>									constexpr encoded_iterator_base(const C* string) : string(string), point(invalid_code_point) {}

		constexpr bool operator== (const encoded_iterator_base& rhs) const { return string == rhs.string; }
		constexpr bool operator!= (const encoded_iterator_base& rhs) const { return string != rhs.string; }
		constexpr bool operator<= (const encoded_iterator_base& rhs) const { return string <= rhs.string; }
		constexpr bool operator>= (const encoded_iterator_base& rhs) const { return string >= rhs.string; }
		constexpr bool operator<  (const encoded_iterator_base& rhs) const { return string < rhs.string; }
		constexpr bool operator>  (const encoded_iterator_base& rhs) const { return string > rhs.string; }
		constexpr bool operator== (const null_iterator& rhs) const { return !string || !string[0]; }
		constexpr bool operator!= (const null_iterator& rhs) const { return string && string[0]; }

	protected:
		const C* string;
		CodePoint point;
	};

	template<encoding::CharacterType C>
	struct encoded_iterator {};

	template<>
	struct encoded_iterator<char8_t> : public encoded_iterator_base<char8_t>
	{
		constexpr encoded_iterator() : encoded_iterator_base(), size(0) {}
		constexpr encoded_iterator(std::nullptr_t) : encoded_iterator_base(), size(0) {}
		template<encoding::SameCharacterType<char8_t> C2>	constexpr encoded_iterator(const C2* string) : encoded_iterator_base(string), size(0) {}
		template<>											constexpr encoded_iterator(const char8_t* string) : encoded_iterator_base(string), size(0) {}

		constexpr CodePoint operator* ()
		{
			set_codepoint();
			return point;
		}
		constexpr const CodePoint* operator-> ()
		{
			set_codepoint();
			return &point;
		}

		constexpr encoded_iterator& operator++ ()
		{
			set_size();
			string += size;
			clear();
			return *this;
		}

		constexpr encoded_iterator operator++ (int)
		{
			set_size();
			auto temp = *this;
			string += size;
			clear();
			return temp;
		}

	private:
		constexpr void set_codepoint()
		{
			if (point.valid() || !string)
				return;

			if ((string[0] & make_mask_ct<char8_t, 7, 7>()) == 0)
			{
				encoding::set_unchecked(point, (char32_t)string[0]);
				size = 1;
				return;
			}

			if ((string[0] & make_mask_ct<char8_t, 5, 7>()) == (0b110 << 5))
			{
				if (encoding::valid_header(string, 1))
				{
					char32_t c = 0;
					c |= ((char32_t)string[0] & make_mask_ct<char32_t, 0, 4>()) << (6 * 1);
					c |= ((char32_t)string[1] & make_mask_ct<char32_t, 0, 5>()) << (6 * 0);
					encoding::set_unchecked(point, c);
					size = 2;
				}
				else
					size = 1;
				return;
			}

			if ((string[0] & make_mask_ct<char8_t, 4, 7>()) == (0b1110 << 4))
			{
				if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2))
				{
					char32_t c = 0;
					c |= ((char32_t)string[0] & make_mask_ct<char32_t, 0, 3>()) << (6 * 2);
					c |= ((char32_t)string[1] & make_mask_ct<char32_t, 0, 5>()) << (6 * 1);
					c |= ((char32_t)string[2] & make_mask_ct<char32_t, 0, 5>()) << (6 * 0);
					encoding::set_unchecked(point, c);
					size = 3;
				}
				else
					size = 1;
				return;
			}

			if ((string[0] & make_mask_ct<char8_t, 3, 7>()) == (0b11110 << 3))
			{
				if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2) && encoding::valid_header(string, 3))
				{
					char32_t c = 0;
					c |= ((char32_t)string[0] & make_mask_ct<char32_t, 0, 3>()) << (6 * 3);
					c |= ((char32_t)string[1] & make_mask_ct<char32_t, 0, 5>()) << (6 * 2);
					c |= ((char32_t)string[2] & make_mask_ct<char32_t, 0, 5>()) << (6 * 1);
					c |= ((char32_t)string[3] & make_mask_ct<char32_t, 0, 5>()) << (6 * 0);
					encoding::set_unchecked(point, c);
					size = 4;
				}
				else
					size = 1;
				return;
			}
		}
		constexpr void set_size()
		{
			if (size || !string)
				return;

			if ((string[0] & make_mask_ct<char8_t, 7, 7>()) == 0)
			{
				size = 1;
				return;
			}

			if ((string[0] & make_mask_ct<char8_t, 5, 7>()) == (0b110 << 5))
			{
				if (encoding::valid_header(string, 1))
					size = 2;
				else
					size = 1;
				return;
			}

			if ((string[0] & make_mask_ct<char8_t, 4, 7>()) == (0b1110 << 4))
			{
				if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2))
					size = 3;
				else
					size = 1;
				return;
			}

			if ((string[0] & make_mask_ct<char8_t, 3, 7>()) == (0b11110 << 3))
			{
				if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2) && encoding::valid_header(string, 3))
					size = 4;
				else
					size = 1;
				return;
			}

			size = 1;
		}

		constexpr void clear()
		{
			point = invalid_code_point;
			size = 0;
		}

		u8 size;
	};

	namespace encoding
	{
		static constexpr std::endian reverse_endianness(std::endian endianness)
		{
			if (endianness == std::endian::little)
				return std::endian::big;
			return std::endian::little;
		}

		static constexpr std::endian get_utf16_endianness(const char16_t* str)
		{
			if (str && str[0] == (char16_t)0xFFFE)
				return reverse_endianness(build.endianness);
			return build.endianness;
		}

		static std::endian get_utf16_endianness(const wchar_t* str)
		{
			return get_utf16_endianness(reinterpret_cast<const char16_t*>(str));
		}


		static constexpr char16_t byte_swap(char16_t source)
		{
			return (((source) >> 8) & 0xFF) | ((((source) & 0xFF) << 8) & 0xFF00);
		}
	}

	template<>
	struct encoded_iterator<char16_t> : public encoded_iterator_base<char16_t>
	{
		constexpr encoded_iterator() : encoded_iterator_base(), size(0), endianness(build.endianness) {}
		constexpr encoded_iterator(std::nullptr_t) : encoded_iterator_base(), size(0), endianness(build.endianness) {}
		template<encoding::SameCharacterType<char16_t> C2>	constexpr encoded_iterator(const C2* string) : encoded_iterator_base(string), size(0), endianness(build.endianness) {}
		template<>											constexpr encoded_iterator(const char16_t* string) : encoded_iterator_base(string), size(0), endianness(build.endianness) {}

		constexpr encoded_iterator(std::endian endianness) : encoded_iterator_base(), size(0), endianness(endianness) {}
		template<encoding::SameCharacterType<char16_t> C2>	constexpr encoded_iterator(const C2* string, std::endian endianness) : encoded_iterator_base(string), size(0), endianness(endianness) {}
		template<>											constexpr encoded_iterator(const char16_t* string, std::endian endianness) : encoded_iterator_base(string), size(0), endianness(endianness) {}

		constexpr CodePoint operator* ()
		{
			set_codepoint();
			return point;
		}
		constexpr const CodePoint* operator-> ()
		{
			set_codepoint();
			return &point;
		}

		constexpr encoded_iterator& operator++ ()
		{
			set_size();
			string += size;
			clear();
			return *this;
		}

		constexpr encoded_iterator operator++ (int)
		{
			set_size();
			auto temp = *this;
			string += size;
			clear();
			return temp;
		}

	private:
		constexpr void set_codepoint()
		{
			if (point.valid() || !string)
				return;

			char16_t W1;
			char16_t W2;

			if (endianness == build.endianness)
			{
				W1 = string[0];
				W2 = string[1];
			}
			else
			{
				W1 = encoding::byte_swap(string[0]);
				W2 = encoding::byte_swap(string[1]);
			}

			if (W1 < 0xD800 || W1 > 0xDFFF)
				encoding::set_unchecked(point, (char32_t)W1);

			else if (W1 >= 0xD800 && W1 <= 0xDBFF && W2 >= 0xDC00 && W2 <= 0xDFFF)
			{
				char32_t c = 0;
				c |= (W1 & make_mask_ct<char32_t, 0, 9>()) << 10;
				c |= (W2 & make_mask_ct<char32_t, 0, 9>()) << 0;
				encoding::set_unchecked(point, c);
				size = 2;
			}

			size = 1;
		}

		constexpr void set_size()
		{
			if (size || !string)
				return;

			char16_t W1;
			char16_t W2;

			if (endianness == build.endianness)
			{
				W1 = string[0];
				W2 = string[1];
			}
			else
			{
				W1 = encoding::byte_swap(string[0]);
				W2 = encoding::byte_swap(string[1]);
			}

			if (W1 < 0xD800 || W1 > 0xDFFF)
				size = 1;

			else if (W1 >= 0xD800 && W1 <= 0xDBFF && W2 >= 0xDC00 && W2 <= 0xDFFF)
				size = 2;

			size = 1;
		}

		constexpr void clear()
		{
			point = invalid_code_point;
			size = 0;
		}

		u8 size;
		std::endian endianness;
	};

	namespace encoding
	{
		template<CharacterType C>
		static constexpr bool valid_encoded_string(const C* string) { return valid_encoded_string(reinterpret_cast<const char_size<sizeof(C)>::type*>(string)); }
		template<>
		static constexpr bool valid_encoded_string(const char8_t* string)
		{
			if (!string)
				return true;

			while (string[0])
				if ((string[0] & make_mask_ct<char8_t, 7, 7>()) == 0)
					string += 1;

				else if ((string[0] & make_mask_ct<char8_t, 5, 7>()) == (0b110 << 5))
					if (encoding::valid_header(string, 1))
						string += 2;
					else
						return false;

				else if ((string[0] & make_mask_ct<char8_t, 4, 7>()) == (0b1110 << 4))
					if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2))
						string += 3;
					else
						return false;

				else if ((string[0] & make_mask_ct<char8_t, 3, 7>()) == (0b11110 << 3))
					if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2) && encoding::valid_header(string, 3))
						string += 4;
					else
						return false;
				else
					return false;

			return true;
		}
		template<>
		static constexpr bool valid_encoded_string(const char16_t* string)
		{
			if (!string)
				return true;

			std::endian endianness = get_utf16_endianness(string);

			while (string[0])
			{
				char16_t W1;
				char16_t W2;

				if (endianness == build.endianness)
				{
					W1 = string[0];
					W2 = string[1];
				}
				else
				{
					W1 = encoding::byte_swap(string[0]);
					W2 = encoding::byte_swap(string[1]);
				}

				if (W1 < 0xD800 || W1 > 0xDFFF)
					string += 1;

				else if (W1 >= 0xD800 && W1 <= 0xDBFF && W2 >= 0xDC00 && W2 <= 0xDFFF)
					string += 2;

				else
					return false;
			}

			return true;
		}
		template<>
		static constexpr bool valid_encoded_string(const char32_t* string)
		{
			if (!string)
				return true;

			while (string[0])
				if (string[0] <= 0x10FFFF)
					++string;
				else
					return false;
			return true;
		}

		template<CharacterType C>
		struct invalid_string { static constexpr C value[] = { (C)-1, (C)0 }; };

		template<CharacterType C>
		static constexpr const C* make_invalid_string() 
		{
			return invalid_string<C>::value;
		}
	}

	template<encoding::CharacterType C>
	struct encoded_cstring
	{
	public:
		constexpr encoded_cstring() : string(nullptr) {}
		constexpr encoded_cstring(std::nullptr_t) : string(nullptr) {}
		template<encoding::SameCharacterType<C> C2>	constexpr encoded_cstring(const C2* string) : string(reinterpret_cast<const C*>(string)) {}
		template<>									constexpr encoded_cstring(const C* string) : string(string) {}

		constexpr bool empty() const { return !string; }
		constexpr size_t character_size() const { if (empty()) return 0; const C* s = string;  while (*s) ++s; return s - string; }
		constexpr size_t size() const { size_t s = 0;  for (auto it = begin(); it != end(); ++it) ++s; return s; }

		constexpr std::endian endianness() const { if constexpr (std::is_same_v<C, char16_t>) return encoding::get_utf16_endianness(string); return build.endianness; }

		constexpr bool operator== (const encoded_cstring& rhs) const { return detail::byte_string_equal(string, rhs.string); }
		constexpr bool operator!= (const encoded_cstring& rhs) const { return detail::byte_string_not_equal(string, rhs.string); }

		constexpr CodePoint operator[] (size_t index) const { return at(index); }
		constexpr CodePoint at(size_t index) const
		{
			auto b = begin();
			auto e = end();
			size_t i = 0;
			auto it = b;
			while (it != e)
			{
				++i;
				++it;
			}
			if (i <= index)
				return *it;
			return invalid_code_point;
		}

		constexpr bool valid() const { return encoding::valid_encoded_string(string); }
		constexpr bool invalid() const { return !valid(); }

		constexpr encoded_iterator<C> begin() const { return string; }
		constexpr null_iterator end() const { return null_iterator(); }

		template<typename CC = C>	constexpr const CC* data()		const { return reinterpret_cast<const CC*>(string); }
		template<>					constexpr const C*  data<C>()	const { return string; }

		template<typename CC = C>	constexpr const CC* c_str()		const { return reinterpret_cast<const CC*>(string); }
		template<>					constexpr const C*  c_str<C>()	const { return string; }

	protected:
		const C* string;
	};

	template<encoding::CharacterType C>
	struct valid_encoded_cstring : public encoded_cstring<C>
	{
		constexpr valid_encoded_cstring() : encoded_cstring<C>() {}
		constexpr valid_encoded_cstring(std::nullptr_t) : encoded_cstring<C>() {}
		template<encoding::SameCharacterType<C> C2>
		constexpr valid_encoded_cstring(const C2* string) : encoded_cstring<C>(encoding::valid_encoded_string<C2>(string) ? string : encoding::make_invalid_string<C2>()) {}
		template<> 
		constexpr valid_encoded_cstring(const C* string) : encoded_cstring<C>(encoding::valid_encoded_string<C>(string) ? string : encoding::make_invalid_string<C>()) {}
		
		constexpr bool valid() const { return this->string != encoding::make_invalid_string<C>(); }
		constexpr bool invalid() const { return !valid(); }
	};

	namespace encoding
	{
		template<typename E>
		static constexpr size_t make_unknown(E(&encoded)[4 / sizeof(E)]);

		static constexpr const char8_t  unknown_utf8[] = u8"\uFFFD";
		static constexpr const char16_t unknown_utf16[] = u"\uFFFD";

		template<>
		static constexpr size_t make_unknown(char8_t(&encoded)[4])
		{
			encoded[0] = unknown_utf8[0];
			encoded[1] = unknown_utf8[1];
			encoded[2] = unknown_utf8[2];
			return 3;
		}

		template<>
		static constexpr size_t make_unknown(char16_t(&encoded)[2])
		{
			encoded[0] = unknown_utf16[0];
			return 1;
		}

		template<typename E, typename C>
		static constexpr size_t to_encoding(E(&encoded)[4 / sizeof(E)], C character);

		template<>
		static constexpr size_t to_encoding(char8_t(&encoded)[4], char8_t character)
		{
			if (character & make_mask_ct<char8_t, 7, 7>())
			{
				encoded[0] = character;
				return 1;
			}
			return make_unknown(encoded);
		}

		template<>
		static constexpr size_t to_encoding(char8_t(&encoded)[4], char16_t character)
		{
			if (character < 0xD800 || character > 0xDFFF)
			{
				if (character <= make_mask_ct<char16_t, 0, 7>())
				{
					encoded[0] = static_cast<char8_t>(character);
					return 1;
				}
				if (character <= make_mask_ct<char16_t, 0, 5 + 1 * 6 - 1>())
				{
					encoded[0] = (0b110 << 5) | static_cast<char8_t>((character >> 6) & make_mask_ct<char16_t, 0, 4>());
					encoded[1] = (0b10  << 6) | static_cast<char8_t>((character >> 0) & make_mask_ct<char16_t, 0, 5>());
					return 2;
				}
				encoded[0] = (0b1110 << 4) | static_cast<char8_t>((character >> (2 * 6)) & make_mask_ct<char16_t, 0, 3>());
				encoded[1] = (0b10   << 6) | static_cast<char8_t>((character >> (1 * 6)) & make_mask_ct<char16_t, 0, 5>());
				encoded[2] = (0b10   << 6) | static_cast<char8_t>((character >> (0 * 6)) & make_mask_ct<char16_t, 0, 5>());
				return 3;
			}
			return make_unknown(encoded);
		}
		template<>
		static constexpr size_t to_encoding(char8_t(&encoded)[4], char32_t character)
		{
			if (character <= make_mask_ct<char32_t, 0, 7>())
			{
				encoded[0] = static_cast<char8_t>(character);
				return 1;
			}
			if (character <= make_mask_ct<char32_t, 0, 5 + 1 * 6 - 1>())
			{
				encoded[0] = (0b110 << 5) | static_cast<char8_t>((character >> 6) & make_mask_ct<char32_t, 0, 4>());
				encoded[1] = (0b10  << 6) | static_cast<char8_t>((character >> 0) & make_mask_ct<char32_t, 0, 5>());
				return 2;
			}
			if (character <= make_mask_ct<char32_t, 0, 4 + 2 * 6 - 1>())
			{
				encoded[0] = (0b1110 << 4) | static_cast<char8_t>((character >> (2 * 6)) & make_mask_ct<char32_t, 0, 3>());
				encoded[1] = (0b10   << 6) | static_cast<char8_t>((character >> (1 * 6)) & make_mask_ct<char32_t, 0, 5>());
				encoded[2] = (0b10   << 6) | static_cast<char8_t>((character >> (0 * 6)) & make_mask_ct<char32_t, 0, 5>());
				return 3;
			}
			if (character <= make_mask_ct<char32_t, 0, 3 + 3 * 6 - 1>())
			{
				encoded[0] = (0b11110 << 3) | static_cast<char8_t>((character >> (3 * 6)) & make_mask_ct<char32_t, 0, 2>());
				encoded[1] = (0b10    << 6) | static_cast<char8_t>((character >> (2 * 6)) & make_mask_ct<char32_t, 0, 5>());
				encoded[2] = (0b10    << 6) | static_cast<char8_t>((character >> (1 * 6)) & make_mask_ct<char32_t, 0, 5>());
				encoded[3] = (0b10    << 6) | static_cast<char8_t>((character >> (0 * 6)) & make_mask_ct<char32_t, 0, 5>());
				return 4;
			}
			return make_unknown(encoded);
		}

		template<>
		static constexpr size_t to_encoding(char16_t(&encoded)[2], char8_t character)
		{
			if (character & make_mask_ct<char8_t, 7, 7>())
			{
				encoded[0] = static_cast<char16_t>(character);
				return 1;
			}
			return make_unknown(encoded);
		}
		template<>
		static constexpr size_t to_encoding(char16_t(&encoded)[2], char16_t character)
		{
			if (character < 0xD800 || character > 0xDFFF)
			{
				encoded[0] = character;
				return 1;
			}
			return make_unknown(encoded);
		}
		template<>
		static constexpr size_t to_encoding(char16_t(&encoded)[2], char32_t character)
		{
			if (character < 0x10000)
			{
				encoded[0] = static_cast<char16_t>(character);
				return 1;
			}
			character -= 0x10000;
			if (character <= 0xFFFFF)
			{
				encoded[0] = 0xD800 | (character >> (10 * 1));
				encoded[1] = 0xDC00 | (character >> (10 * 0));
				return 2;
			}
			return make_unknown(encoded);
		}

		template<CharacterType C>
		static void reserve_string(std::basic_string<C>& string, size_t size)
		{
			size_t newSize = size + string.size();
			if (newSize > string.capacity())
				string.reserve(newSize);
		}

		template<CharacterType C>
		static size_t string_length(const C* str)
		{
			if (!str)
				return 0;
			size_t size = 0;
			while (str[size])
				++size;
			return size;
		}
		template<CharacterType C>
		static void append_unknown(std::basic_string<char8_t>& stdstring, const C*& string)
		{
			stdstring.append(unknown_utf8, 3);
			string += 1;
		}
		template<CharacterType C>
		static void append_unknown(std::basic_string<char16_t>& stdstring, const C*& string)
		{
			stdstring.append(unknown_utf16, 1);
			string += 1;
		}
		template<typename E, typename C>
		static void append_stdstring_size(std::basic_string<E>& std, const C* string, size_t size);
		template<>
		static void append_stdstring_size(std::basic_string<char8_t>& stdstring, const char8_t* string, size_t size)
		{
			reserve_string(stdstring, size * 3);
			const char8_t* end = string + size;
			while (string < end)
			{
				if ((string[0] & make_mask_ct<char8_t, 7, 7>()) == 0)
				{
					stdstring.append(string, 1);
					string += 1;
				}

				else if ((string[0] & make_mask_ct<char8_t, 5, 7>()) == (0b110 << 5))
					if (encoding::valid_header(string, 1))
					{
						stdstring.append(string, 2);
						string += 2;
					}
					else
						append_unknown(stdstring, string);

				else if ((string[0] & make_mask_ct<char8_t, 4, 7>()) == (0b1110 << 4))
					if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2))
					{
						stdstring.append(string, 3);
						string += 3;
					}
					else
						append_unknown(stdstring, string);

				else if ((string[0] & make_mask_ct<char8_t, 3, 7>()) == (0b11110 << 3))
					if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2) && encoding::valid_header(string, 3))
					{
						stdstring.append(string, 4);
						string += 4;
					}
					else
						append_unknown(stdstring, string);
				else
					append_unknown(stdstring, string);
			}
		}
		template<>
		static void append_stdstring_size(std::basic_string<char8_t>& stdstring, const char16_t* string, size_t size)
		{
			reserve_string(stdstring, size * 3);
			const char16_t* end = string + size;
			std::endian endianness = get_utf16_endianness(string);
			char8_t encoded[4]{};

			while (string < end)
			{
				char16_t W1;
				char16_t W2;

				if (endianness == build.endianness)
				{
					W1 = string[0];
					W2 = string[1];
				}
				else
				{
					W1 = encoding::byte_swap(string[0]);
					W2 = encoding::byte_swap(string[1]);
				}

				if (W1 < 0xD800 || W1 > 0xDFFF)
				{
					if (W1 <= make_mask_ct<char16_t, 0, 7>())
					{
						encoded[0] = static_cast<char8_t>(W1);
						stdstring.append(encoded, 1);
					}
					else if (W1 <= make_mask_ct<char16_t, 0, 5 + 1 * 6 - 1>())
					{
						encoded[0] = (0b110 << 5) | static_cast<char8_t>((W1 >> 6) & make_mask_ct<char16_t, 0, 4>());
						encoded[1] = (0b10 << 6)  | static_cast<char8_t>((W1 >> 0) & make_mask_ct<char16_t, 0, 5>());
						stdstring.append(encoded, 2);
					}
					else
					{
						encoded[0] = (0b1110 << 4) | static_cast<char8_t>((W1 >> (2 * 6)) & make_mask_ct<char16_t, 0, 3>());
						encoded[1] = (0b10 << 6)   | static_cast<char8_t>((W1 >> (1 * 6)) & make_mask_ct<char16_t, 0, 5>());
						encoded[2] = (0b10 << 6)   | static_cast<char8_t>((W1 >> (0 * 6)) & make_mask_ct<char16_t, 0, 5>());
						stdstring.append(encoded, 3);
					}
					string += 1;
				}

				else if (W1 >= 0xD800 && W1 <= 0xDBFF && W2 >= 0xDC00 && W2 <= 0xDFFF)
				{
					char32_t c = 0;
					c |= (W1 & make_mask_ct<char32_t, 0, 9>()) << 10;
					c |= (W2 & make_mask_ct<char32_t, 0, 9>()) << 0;
					size_t used = to_encoding(encoded, c);
					stdstring.append(encoded, used);
					string += 2;
				}
				else
					append_unknown(stdstring, string);
			}
		}
		template<>
		static void append_stdstring_size(std::basic_string<char8_t>& stdstring, const char32_t* string, size_t size)
		{
			reserve_string(stdstring, size * 4);
			const char32_t* end = string + size;
			char8_t encoded[4]{};
			while (string < end)
			{
				size_t used = to_encoding(encoded, string[0]);
				stdstring.append(encoded, used);
				string += 1;
			}
		}
		template<>
		static void append_stdstring_size(std::basic_string<char16_t>& stdstring, const char8_t* string, size_t size)
		{
			reserve_string(stdstring, size);
			const char8_t* end = string + size;

			while (string < end)
				if ((string[0] & make_mask_ct<char8_t, 7, 7>()) == 0)
				{
					stdstring.push_back(static_cast<char16_t>(string[0]));
					string += 1;
				}

				else if ((string[0] & make_mask_ct<char8_t, 5, 7>()) == (0b110 << 5))
					if (encoding::valid_header(string, 1))
					{
						char16_t c = 0;
						c |= ((char16_t)string[0] & make_mask_ct<char16_t, 0, 4>()) << (6 * 1);
						c |= ((char16_t)string[1] & make_mask_ct<char16_t, 0, 5>()) << (6 * 0);
						stdstring.push_back(c);
						string += 2;
					}
					else
						append_unknown(stdstring, string);

				else if ((string[0] & make_mask_ct<char8_t, 4, 7>()) == (0b1110 << 4))
					if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2))
					{
						char32_t c = 0;
						c |= ((char32_t)string[0] & make_mask_ct<char32_t, 0, 3>()) << (6 * 2);
						c |= ((char32_t)string[1] & make_mask_ct<char32_t, 0, 5>()) << (6 * 1);
						c |= ((char32_t)string[2] & make_mask_ct<char32_t, 0, 5>()) << (6 * 0);
						char16_t encoded[2]{};
						size_t used = to_encoding(encoded, c);
						stdstring.append(encoded, used);
						stdstring += 3;
					}
					else
						append_unknown(stdstring, string);

				else if ((string[0] & make_mask_ct<char8_t, 3, 7>()) == (0b11110 << 3))
					if (encoding::valid_header(string, 1) && encoding::valid_header(string, 2) && encoding::valid_header(string, 3))
					{
						char32_t c = 0;
						c |= ((char32_t)string[0] & make_mask_ct<char32_t, 0, 3>()) << (6 * 3);
						c |= ((char32_t)string[1] & make_mask_ct<char32_t, 0, 5>()) << (6 * 2);
						c |= ((char32_t)string[2] & make_mask_ct<char32_t, 0, 5>()) << (6 * 1);
						c |= ((char32_t)string[3] & make_mask_ct<char32_t, 0, 5>()) << (6 * 0);
						char16_t encoded[2]{};
						size_t used = to_encoding(encoded, c);
						stdstring.append(encoded, used);
						stdstring += 4;
					}
					else
						append_unknown(stdstring, string);
				else
					append_unknown(stdstring, string);

		}
		template<>
		static void append_stdstring_size(std::basic_string<char16_t>& stdstring, const char16_t* string, size_t size)
		{
			reserve_string(stdstring, size);
			const char16_t* end = string + size;

			char16_t W1;
			char16_t W2;
			std::endian endianness = get_utf16_endianness(string);

			while (string < end)
			{
				if (endianness == build.endianness)
				{
					W1 = string[0];
					W2 = string[1];
				}
				else
				{
					W1 = encoding::byte_swap(string[0]);
					W2 = encoding::byte_swap(string[1]);
				}

				if (W1 < 0xD800 || W1 > 0xDFFF)
				{
					stdstring.append(string, 1);
					string += 1;
				}

				else if (W1 >= 0xD800 && W1 <= 0xDBFF && W2 >= 0xDC00 && W2 <= 0xDFFF)
				{
					stdstring.append(string, 2);
					string += 2;
				}
				else
					append_unknown(stdstring, string);
			}
		}
		template<>
		static void append_stdstring_size(std::basic_string<char16_t>& stdstring, const char32_t* string, size_t size)
		{
			reserve_string(stdstring, size * 2);
			const char32_t* end = string + size;
			char16_t encoded[2]{};
			while (string < end)
			{
				size_t used = to_encoding(encoded, string[0]);
				stdstring.append(encoded, used);
				string += 1;
			}
		}

		template<typename E, typename C>
		static void append_stdstring(std::basic_string<E>& stdstring, const C* string)
		{
			if (!string)
				return;
			size_t size = string_length(string);
			append_stdstring_size(stdstring, string, size);
		}
	}

	template<encoding::CharacterType C>
	class encoded_string
	{
	public:
		constexpr encoded_string() : string() {}
		constexpr encoded_string(std::nullptr_t) : string() {}
		template<encoding::CharacterType C2>	encoded_string(const C2* string) : string() { append(string); }
		template<encoding::CharacterType C2>	encoded_string(valid_encoded_cstring<C2> string) : string() { append(string); }
		template<encoding::CharacterType C2>	encoded_string(const std::basic_string<C2>& string) : string() { append(string); }
		template<encoding::CharacterType C2>	encoded_string(encoded_cstring<C2> string) : string() { append(string); }
		template<encoding::CharacterType C2>	encoded_string(const encoded_string<C2>& string) : string() { append(string); }
		template<>
		encoded_string(const encoded_string& rhs) : string(rhs.string) {}
		encoded_string(encoded_string&& rhs) noexcept : string(std::move(rhs.string)) {}

		encoded_string& operator= (const encoded_string& rhs) { string = rhs.string; return *this; }
		encoded_string& operator= (encoded_string&& rhs) { string = std::move(rhs.string); return *this; }

		template<typename C2>
		requires(encoding::CharacterType<C2> || std::is_same_v<C2, CodePoint>)
		void push_back(C2 character) { C encoded[4 / sizeof(C)]{}; string.append(encoded, encoding::to_encoding(encoded, static_cast<typename encoding::char_size<sizeof(C2)>::type>(character))); }

		template<encoding::SameCharacterType<C> CC = C> constexpr const CC* data()		const { return reinterpret_cast<const CC*>(string.data()); }
		template<>										constexpr const C*  data<C>()	const { return string.data(); }

		template<encoding::SameCharacterType<C> CC = C>	constexpr const CC* c_str()		const { return reinterpret_cast<const CC*>(string.c_str()); }
		template<>										constexpr const C*  c_str<C>()	const { return string.c_str(); }

		constexpr encoded_cstring<C> encoded_cstring() const { return string.c_str(); }

		void append(char character) { push_back(character); }
		void append(wchar_t character) { push_back(character); }
		void append(char8_t character) { push_back(character); }
		void append(char16_t character) { push_back(character); }
		void append(char32_t character) { push_back(character); }
		void append(CodePoint character) { push_back(character); }
		template<encoding::CharacterType C2>	void append(const C2* string) { encoding::append_stdstring(this->string, reinterpret_cast<const typename encoding::char_size<sizeof(C2)>::type*>(string)); }
		template<encoding::CharacterType C2>	void append(valid_encoded_cstring<C2> string) { if constexpr (sizeof(C2) == sizeof(C)) { if (string.valid()) this->string.append(reinterpret_cast<const C*>(string.c_str())); } else if (string.valid()) append(string.c_str()); }
		template<encoding::CharacterType C2>	void append(rv::encoded_cstring<C2> string) { append(string.c_str()); }
		template<encoding::CharacterType C2>	void append(const std::basic_string<C2>& string) { encoding::append_stdstring_size(this->string, reinterpret_cast<const typename encoding::char_size<sizeof(C2)>::type*>(string.data()), string.size()); }
		template<encoding::CharacterType C2>	void append(const encoded_string<C2>& string) { if constexpr (sizeof(C2) == sizeof(C)) this->string.append(reinterpret_cast<const C*>(string.c_str()), string.character_size()); else encoding::append_stdstring_size(this->string, reinterpret_cast<const typename encoding::char_size<sizeof(C2)>::type*>(string.data()), string.size()); }

		template<encoding::CharacterType C2>	encoded_string& operator+= (const C2* string)						{ append(string); return *this; }
		template<encoding::CharacterType C2>	encoded_string& operator+= (valid_encoded_cstring<C2> string)		{ append(string); return *this; }
		template<encoding::CharacterType C2>	encoded_string& operator+= (rv::encoded_cstring<C2> string)			{ append(string); return *this; }
		template<encoding::CharacterType C2>	encoded_string& operator+= (const std::basic_string<C2>& string)	{ append(string); return *this; }
		template<encoding::CharacterType C2>	encoded_string& operator+= (const encoded_string<C2>& string)		{ append(string); return *this; }
		encoded_string& operator+= (char character)			{ push_back(character); return *this; }
		encoded_string& operator+= (wchar_t character)		{ push_back(character); return *this; }
		encoded_string& operator+= (char8_t character)		{ push_back(character); return *this; }
		encoded_string& operator+= (char16_t character)		{ push_back(character); return *this; }
		encoded_string& operator+= (char32_t character)		{ push_back(character); return *this; }
		encoded_string& operator+= (CodePoint character)	{ push_back(character); return *this; }

		encoded_iterator<C> begin()	const { return data(); }
		encoded_iterator<C> end()	const { return data() + string.size(); }

		constexpr bool empty() const { return string.empty(); }
		constexpr size_t character_size() const { return string.size(); }
		constexpr size_t size() const { size_t s = 0;  for (auto it = begin(); it != end(); ++it) ++s; return s; }

		constexpr CodePoint operator[] (size_t index) const { return at(index); }
		constexpr CodePoint at(size_t index) const
		{
			auto b = begin();
			auto e = end();
			size_t i = 0;
			auto it = b;
			while (it != e)
			{
				++i;
				++it;
			}
			if (i <= index)
				return *it;
			return invalid_code_point;
		}

		const std::basic_string<C>& std_string() const { return string; }

	private:
		std::basic_string<C> string;
	};

	typedef encoded_cstring<char8_t>		utf8_cstring;
	typedef encoded_cstring<char16_t>		utf16_cstring;

	typedef encoded_string<char8_t>			utf8_string;
	typedef encoded_string<char16_t>		utf16_string;

	typedef valid_encoded_cstring<char8_t>	valid_utf8_cstring;
	typedef valid_encoded_cstring<char16_t>	valid_utf16_cstring;

	namespace detail
	{
		template<encoding::CharacterType C, typename T>
		std::basic_ostream<C>& make_str(std::basic_ostream<C>& os, const T& object)
		{
			if constexpr (encoding::CharacterType<T>)
			{
				using C2 = typename encoding::char_size<sizeof(C)>::type;
				using C3 = typename encoding::char_size<sizeof(T)>::type;
				C2 encoded[4 / sizeof(C2)]{};
				size_t used = encoding::to_encoding(encoded, static_cast<C3>(object));				
				return os.write(reinterpret_cast<const C*>(encoded), used);
			}
			else
				return os << object;
		}

		template<encoding::CharacterType C1, encoding::CharacterType C2>
		std::basic_ostream<C1>& make_str (std::basic_ostream<C1>& os, const encoded_string<C2>& str)
		{
			if constexpr (sizeof(C1) == sizeof(C2))
				return os.write(str.c_str<C1>(), str.character_size());
			else
				return make_str(os, encoded_string<typename encoding::char_size<sizeof(C1)>::type>(str));
		}

		template<encoding::CharacterType C1, encoding::CharacterType C2>
		std::basic_ostream<C1>& make_str(std::basic_ostream<C1>& os, const std::basic_string<C2>& str)
		{
			if constexpr (sizeof(C1) == sizeof(C2))
				return os.write(reinterpret_cast<C1>(str.c_str()), str.size());
			else
				return make_str(os, encoded_string<typename encoding::char_size<sizeof(C1)>::type>(str));
		}

		template<encoding::CharacterType C1, encoding::CharacterType C2>
		std::basic_ostream<C1>& make_str(std::basic_ostream<C1>& os, const C2* str)
		{
			if constexpr (sizeof(C1) == sizeof(C2))
				if (str)
					return os << reinterpret_cast<const C1*>(str);
				else
					return os;
			else
				return make_str(os, encoded_string<typename encoding::char_size<sizeof(C1)>::type>(str));
		}

		template<encoding::CharacterType C1, typename C2>
		std::basic_ostream<C1>& make_str(std::basic_ostream<C1>& os, C2* str)
		{
			if constexpr (encoding::CharacterType<C2>)
				return make_str(os, reinterpret_cast<const C2*>(str));
			else
				return os << str;
		}

		template<encoding::CharacterType C1, encoding::CharacterType C2>
		std::basic_ostream<C1>& make_str(std::basic_ostream<C1>& os, encoded_cstring<C2> str)
		{
			return make_str(os, str.c_str());
		}

		template<encoding::CharacterType C1, encoding::CharacterType C2>
		std::basic_ostream<C1>& make_str(std::basic_ostream<C1>& os, valid_encoded_cstring<C2> str)
		{
			return make_str(os, str.c_str());
		}

		template<typename C = char, typename F, typename... Args>
		void str(std::basic_ostringstream<C>& oss, const F& first, const Args&... args)
		{
			make_str(oss, first);
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

	template<typename... Args>
	std::basic_string<char> str8(const Args&... args)
	{
		return str<char>(args...);
	}

	template<typename... Args>
	std::basic_string<wchar_t> str16(const Args&... args)
	{
		return str<wchar_t>(args...);
	}

	template<encoding::CharacterType C>
	static constexpr valid_encoded_cstring<C> strvalid(const C* str) { return str; }
}