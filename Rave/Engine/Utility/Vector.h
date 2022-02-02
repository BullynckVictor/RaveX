#pragma once
#include "Engine/Utility/Concepts.h"
#include <array>
#include <type_traits>

namespace rv
{
	template<typename P, typename T>
	struct VectorBase
	{
		static constexpr size_t size() { return sizeof(P) / sizeof(T); }

				T* data()							{ return reinterpret_cast<T*>(this); }
		const	T* data()					const	{ return reinterpret_cast<const T*>(this); }
				T* begin()							{ return data(); }
		const	T* begin()					const	{ return data(); }
		const	T* cbegin()					const	{ return data(); }
				T* end()							{ return data() + size(); }
		const	T* end()					const	{ return data() + size(); }
		const	T* cend()					const	{ return data() + size(); }
				T* rbegin()							{ return data() + size() - 1; }
		const	T* rbegin()					const	{ return data() + size() - 1; }
		const	T* crbegin()				const	{ return data() + size() - 1; }
				T* rend()							{ return data() - 1; }
		const	T* rend()					const	{ return data() - 1; }
		const	T* crend()					const	{ return data() - 1; }
				T& operator[](size_t index)			{ return data()[index]; }
		const	T& operator[](size_t index)	const	{ return data()[index]; }
	};

	template<typename V>					concept VectorType = std::is_base_of_v<VectorBase<V, typename V::value_type>, V> && std::is_arithmetic_v<typename V::value_type> && !std::is_pointer_v<typename V::value_type>;
	template<typename V1, typename V2>		concept CompatibleVectorType = VectorType<V1> && VectorType<V2> && ConvertibleType<typename V1::value_type, typename V2::value_type>;
	template<typename V1, typename V2>		concept SmallerVectorType = CompatibleVectorType<V1, V1> && V1::size() <= V2::size();
	template<typename V1, typename V2>		concept SameSizeVectorType = CompatibleVectorType<V1, V1> && V1::size() == V2::size();
	template<typename V, typename... Args>	concept ConvertibleArgs = VectorType<V> && sizeof...(Args) == V::size() && detail::all_convertible_and_arithmetic<typename V::value_type, Args...>();

	namespace detail
	{
		template<VectorType V1, SmallerVectorType<V1> V2, size_t D = 0>	static constexpr V1& copy_vectors(V1& lhs, const V2& rhs);
		template<VectorType V1, SmallerVectorType<V1> V2, size_t D = 0>	static constexpr V1& move_vectors(V1& lhs, V2&& rhs);
	}

	template<size_t S, typename T>
	struct Vector : public VectorBase<Vector<S, T>, T>
	{
		using value_type = T;

		constexpr Vector() : array() {}
		template<ConvertibleType<T> C>				constexpr Vector(const C& value) : array() { array.fill(static_cast<T>(value)); }
		template<SmallerVectorType<Vector<S, T>> V>	constexpr Vector(const V& rhs) : array() { *this = rhs; }
		template<SmallerVectorType<Vector<S, T>> V>	constexpr Vector(V&& rhs) : array() { *this = std::move(rhs); }

		template<typename... Args> requires(ConvertibleArgs<Vector<S, T>, Args...>)	
		constexpr Vector(const Args&... args) : array({ static_cast<T>(args)... }) {}

		template<size_t E> constexpr		T& get_element() { return array[E]; }
		template<size_t E> constexpr const	T& get_element() const { return array[E]; }

		template<SmallerVectorType<Vector<S, T>> V> constexpr Vector& operator= (const V& rhs)	{ return detail::copy_vectors(*this, rhs); }
		template<SmallerVectorType<Vector<S, T>> V> constexpr Vector& operator= (V&& rhs)		{ return detail::move_vectors(*this, std::move(rhs)); }

		std::array<T, S> array;
	};

	template<typename T>
	struct Vector<2, T> : public VectorBase<Vector<2, T>, T>
	{
		using value_type = T;

		constexpr Vector() : x(0), y(0) {}
		template<ConvertibleType<T> C>				constexpr Vector(const C& value) : x(static_cast<T>(value)), y(static_cast<T>(value)) {}
		template<SmallerVectorType<Vector<2, T>> V>	constexpr Vector(const V& rhs) : x(0), y(0) { *this = rhs; }
		template<SmallerVectorType<Vector<2, T>> V>	constexpr Vector(V&& rhs) : x(0), y(0) { *this = std::move(rhs); }

		template<ConvertibleType<T> CX, ConvertibleType<T> CY>	
		constexpr Vector(const CX& x, const CY& y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}

		template<size_t E>	constexpr		T& get_element();
		template<size_t E>	constexpr const	T& get_element() const;
		template<>			constexpr		T& get_element<0>()			{ return x; }
		template<>			constexpr const	T& get_element<0>() const	{ return x; }
		template<>			constexpr		T& get_element<1>()			{ return y; }
		template<>			constexpr const	T& get_element<1>() const	{ return y; }

		template<SmallerVectorType<Vector<2, T>> V> constexpr Vector& operator= (const V& rhs)	{ return detail::copy_vectors(*this, rhs); }
		template<SmallerVectorType<Vector<2, T>> V> constexpr Vector& operator= (V&& rhs)		{ return detail::move_vectors(*this, std::move(rhs)); }

		T x;
		T y;
	};

	template<typename T>
	struct Vector<3, T> : public VectorBase<Vector<3, T>, T>
	{
		using value_type = T;

		constexpr Vector() : x(0), y(0), z(0) {}
		template<ConvertibleType<T> C>							constexpr Vector(const C& value) : x(static_cast<T>(value)), y(static_cast<T>(value)), z(static_cast<T>(value)) {}
		template<SmallerVectorType<Vector<3, T>> V>				constexpr Vector(const V& rhs) : x(0), y(0), z(0) { *this = rhs; }
		template<SmallerVectorType<Vector<3, T>> V>				constexpr Vector(V&& rhs) : x(0), y(0), z(0) { *this = std::move(rhs); }

		template<ConvertibleType<T> CX, ConvertibleType<T> CY, ConvertibleType<T> CZ>
		constexpr Vector(const CX& x, const CY& y, const CZ& z) : x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)) {}

		template<size_t E>	constexpr		T& get_element();
		template<size_t E>	constexpr const	T& get_element() const;
		template<>			constexpr		T& get_element<0>()			{ return x; }
		template<>			constexpr const	T& get_element<0>() const	{ return x; }
		template<>			constexpr		T& get_element<1>()			{ return y; }
		template<>			constexpr const	T& get_element<1>() const	{ return y; }
		template<>			constexpr		T& get_element<2>()			{ return z; }
		template<>			constexpr const	T& get_element<2>() const	{ return z; }

		template<SmallerVectorType<Vector<3, T>> V> constexpr Vector& operator= (const V& rhs)	{ return detail::copy_vectors(*this, rhs); }
		template<SmallerVectorType<Vector<3, T>> V> constexpr Vector& operator= (V&& rhs)		{ return detail::move_vectors(*this, std::move(rhs)); }

		T x;
		T y;
		T z;
	};

	template<typename T>
	struct Vector<4, T> : public VectorBase<Vector<4, T>, T>
	{
		using value_type = T;

		constexpr Vector() : x(0), y(0), z(0), w(0) {}
		template<ConvertibleType<T> C>							constexpr Vector(const C& value) : x(static_cast<T>(value)), y(static_cast<T>(value)), z(static_cast<T>(value)), w(static_cast<T>(value)) {}
		template<SmallerVectorType<Vector<4, T>> V>				constexpr Vector(const V& rhs) : x(0), y(0), z(0), w(0) { *this = rhs; }
		template<SmallerVectorType<Vector<4, T>> V>				constexpr Vector(V&& rhs) : x(0), y(0), z(0), w(0) { *this = std::move(rhs); }

		template<ConvertibleType<T> CX, ConvertibleType<T> CY, ConvertibleType<T> CZ, ConvertibleType<T> CW>
		constexpr Vector(const CX& x, const CY& y, const CZ& z, const CW& w) : x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)), w(static_cast<T>(w)) {}

		template<size_t E>	constexpr		T& get_element();
		template<size_t E>	constexpr const	T& get_element() const;
		template<>			constexpr		T& get_element<0>()			{ return x; }
		template<>			constexpr const	T& get_element<0>() const	{ return x; }
		template<>			constexpr		T& get_element<1>()			{ return y; }
		template<>			constexpr const	T& get_element<1>() const	{ return y; }
		template<>			constexpr		T& get_element<2>()			{ return z; }
		template<>			constexpr const	T& get_element<2>() const	{ return z; }
		template<>			constexpr		T& get_element<3>()			{ return w; }
		template<>			constexpr const	T& get_element<3>() const	{ return w; }

		template<SmallerVectorType<Vector<4, T>> V> constexpr Vector& operator= (const V& rhs)	{ return detail::copy_vectors(*this, rhs); }
		template<SmallerVectorType<Vector<4, T>> V> constexpr Vector& operator= (V&& rhs)		{ return detail::move_vectors(*this, std::move(rhs)); }

		T x;
		T y;
		T z;
		T w;
	};

	template<size_t S, typename T>
	struct Extent : public VectorBase<Extent<S, T>, T>
	{
		using value_type = T;

		constexpr Extent() : array() {}
		template<ConvertibleType<T> C>				constexpr Extent(const C& value) : array() { array.fill(static_cast<T>(value)); }
		template<SmallerVectorType<Extent<S, T>> V>	constexpr Extent(const V& rhs) : array() { *this = rhs; }
		template<SmallerVectorType<Extent<S, T>> V>	constexpr Extent(V&& rhs) : array() { *this = std::move(rhs); }

		template<typename... Args> requires(ConvertibleArgs<Vector<S, T>, Args...>)
		constexpr Extent(const Args&... args) : array({ static_cast<T>(args)... }) {}

		template<size_t E> constexpr		T& get_element()		{ return array[E]; }
		template<size_t E> constexpr const	T& get_element() const	{ return array[E]; }

		template<SmallerVectorType<Extent<S, T>> V> constexpr Extent& operator= (const V& rhs) { return detail::copy_vectors(*this, rhs); }
		template<SmallerVectorType<Extent<S, T>> V> constexpr Extent& operator= (V&& rhs) { return detail::move_vectors(*this, std::move(rhs)); }

		std::array<T, S> array;
	};

	template<typename T>
	struct Extent<2, T> : public VectorBase<Extent<2, T>, T>
	{
		using value_type = T;

		constexpr Extent() : width(0), height(0) {}
		template<ConvertibleType<T> C>				constexpr Extent(const C& value) : width(static_cast<T>(value)), height(static_cast<T>(value)) {}
		template<SmallerVectorType<Extent<2, T>> V>	constexpr Extent(const V& rhs) : width(0), height(0) { *this = rhs; }
		template<SmallerVectorType<Extent<2, T>> V>	constexpr Extent(V&& rhs) : width(0), height(0) { *this = std::move(rhs); }

		template<ConvertibleType<T> CW, ConvertibleType<T> CH>
		constexpr Extent(const CW& width, const CH& height) : width(static_cast<T>(width)), height(static_cast<T>(height)) {}

		template<size_t E>	constexpr		T& get_element();
		template<size_t E>	constexpr const	T& get_element() const;
		template<>			constexpr		T& get_element<0>()			{ return width; }
		template<>			constexpr const	T& get_element<0>() const	{ return width; }
		template<>			constexpr		T& get_element<1>()			{ return height; }
		template<>			constexpr const	T& get_element<1>() const	{ return height; }

		template<SmallerVectorType<Extent<2, T>> V> constexpr Extent& operator= (const V& rhs) { return detail::copy_vectors(*this, rhs); }
		template<SmallerVectorType<Extent<2, T>> V> constexpr Extent& operator= (V&& rhs) { return detail::move_vectors(*this, std::move(rhs)); }

		T width;
		T height;
	};

	template<typename T>
	struct Extent<3, T> : public VectorBase<Extent<3, T>, T>
	{
		using value_type = T;

		constexpr Extent() : width(0), height(0), depth(0) {}
		template<ConvertibleType<T> C>				constexpr Extent(const C& value) : width(static_cast<T>(value)), height(static_cast<T>(value)), depth(static_cast<T>(value)) {}
		template<SmallerVectorType<Extent<3, T>> V>	constexpr Extent(const V& rhs) : width(0), height(0), depth(0) { *this = rhs; }
		template<SmallerVectorType<Extent<3, T>> V>	constexpr Extent(V&& rhs) : width(0), height(0), depth(0) { *this = std::move(rhs); }

		template<ConvertibleType<T> CW, ConvertibleType<T> CH, ConvertibleType<T> CD>
		constexpr Extent(const CW& width, const CH& height, const CD& depth) : width(static_cast<T>(width)), height(static_cast<T>(height)), depth(static_cast<T>(depth)) {}

		template<size_t E>	constexpr		T& get_element();
		template<size_t E>	constexpr const	T& get_element() const;
		template<>			constexpr		T& get_element<0>()			{ return width; }
		template<>			constexpr const	T& get_element<0>() const	{ return width; }
		template<>			constexpr		T& get_element<1>()			{ return height; }
		template<>			constexpr const	T& get_element<1>() const	{ return height; }
		template<>			constexpr		T& get_element<2>()			{ return depth; }
		template<>			constexpr const	T& get_element<2>() const	{ return depth; }

		template<SmallerVectorType<Extent<3, T>> V> constexpr Extent& operator= (const V& rhs) { return detail::copy_vectors(*this, rhs); }
		template<SmallerVectorType<Extent<3, T>> V> constexpr Extent& operator= (V&& rhs) { return detail::move_vectors(*this, std::move(rhs)); }

		T width;
		T height;
		T depth;
	};

	namespace detail
	{
		template<VectorType V1, SmallerVectorType<V1> V2, size_t D>
		static constexpr V1& copy_vectors(V1& lhs, const V2& rhs)
		{
			lhs.get_element<D>() = rhs.get_element<D>();
			if constexpr (D < V2::size() - 1)
				return copy_vectors<V1, V2, D + 1>(lhs, rhs);
			else
				return lhs;
		}

		template<VectorType V1, SmallerVectorType<V1> V2, size_t D>
		static constexpr V1& move_vectors(V1& lhs, V2&& rhs)
		{
			lhs.get_element<D>() = std::move(rhs.get_element<D>());
			if constexpr (D < V2::size() - 1)
				return copy_vectors<V1, V2, D + 1>(lhs, rhs);
			else
				return lhs;
		}

		template<typename T1, ConvertibleType<T1> T2> static constexpr void add_self(T1& a, const T2& b) { a += static_cast<T1>(b); }
		template<typename T1, ConvertibleType<T1> T2> static constexpr void sub_self(T1& a, const T2& b) { a -= static_cast<T1>(b); }
		template<typename T1, ConvertibleType<T1> T2> static constexpr void mul_self(T1& a, const T2& b) { a *= static_cast<T1>(b); }
		template<typename T1, ConvertibleType<T1> T2> static constexpr void div_self(T1& a, const T2& b) { a /= static_cast<T1>(b); }

		template<VectorType V, ConvertibleType<typename V::value_type> T, void(*op)(typename V::value_type&, const T&), size_t D = 0>
		static constexpr void do_operation_constant(V& vector, const T& value)
		{
			op(vector.get_element<D>(), value);
			if constexpr (D < V::size() - 1)
				do_operation_constant<V, T, op, D + 1>(vector, value);
		}

		template<VectorType V1, SmallerVectorType<V1> V2, void(*op)(typename V1::value_type&, const typename V2::value_type&), size_t D = 0>
		static constexpr void do_operation_vector(V1& lhs, const V2& rhs)
		{
			op(lhs.get_element<D>(), rhs.get_element<D>());
			if constexpr (D < V2::size() - 1)
				do_operation_vector<V1, V2, op, D + 1>(lhs, rhs);
		}
	}

	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V& operator+= (V& vector, const T& value) { detail::do_operation_constant<V, T, detail::add_self>(vector, value); return vector; }
	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V& operator-= (V& vector, const T& value) { detail::do_operation_constant<V, T, detail::sub_self>(vector, value); return vector; }
	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V& operator*= (V& vector, const T& value) { detail::do_operation_constant<V, T, detail::mul_self>(vector, value); return vector; }
	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V& operator/= (V& vector, const T& value) { detail::do_operation_constant<V, T, detail::div_self>(vector, value); return vector; }

	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V operator+ (const V& vector, const T& value) { V vector2 = vector; vector2 += value; return vector2; }
	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V operator- (const V& vector, const T& value) { V vector2 = vector; vector2 -= value; return vector2; }
	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V operator* (const V& vector, const T& value) { V vector2 = vector; vector2 *= value; return vector2; }
	template<VectorType V, ConvertibleType<typename V::value_type> T> static constexpr V operator/ (const V& vector, const T& value) { V vector2 = vector; vector2 /= value; return vector2; }

	template<VectorType V1,  SmallerVectorType<V1> V2> static constexpr V1& operator+= (V1& lhs, const V2& rhs) { detail::do_operation_vector<V1, V2, detail::add_self>(lhs, rhs); return lhs; }
	template<VectorType V1,  SmallerVectorType<V1> V2> static constexpr V1& operator-= (V1& lhs, const V2& rhs) { detail::do_operation_vector<V1, V2, detail::sub_self>(lhs, rhs); return lhs; }
	template<VectorType V1, SameSizeVectorType<V1> V2> static constexpr V1& operator+= (V1& lhs, const V2& rhs) { detail::do_operation_vector<V1, V2, detail::mul_self>(lhs, rhs); return lhs; }
	template<VectorType V1, SameSizeVectorType<V1> V2> static constexpr V1& operator-= (V1& lhs, const V2& rhs) { detail::do_operation_vector<V1, V2, detail::div_self>(lhs, rhs); return lhs; }

	template<VectorType V1,  SmallerVectorType<V1> V2> static constexpr V1 operator+ (const V1& lhs, const V2& rhs) { V1 vector2 = lhs; vector2 += rhs; return vector2; }
	template<VectorType V1,  SmallerVectorType<V1> V2> static constexpr V1 operator- (const V1& lhs, const V2& rhs) { V1 vector2 = lhs; vector2 -= rhs; return vector2; }
	template<VectorType V1, SameSizeVectorType<V1> V2> static constexpr V1 operator* (const V1& lhs, const V2& rhs) { V1 vector2 = lhs; vector2 *= rhs; return vector2; }
	template<VectorType V1, SameSizeVectorType<V1> V2> static constexpr V1 operator/ (const V1& lhs, const V2& rhs) { V1 vector2 = lhs; vector2 /= rhs; return vector2; }

	typedef Vector<2, float> Vector2;
	typedef Vector<3, float> Vector3;
	typedef Vector<4, float> Vector4;

	typedef Extent<2, float> Extent2;
	typedef Extent<3, float> Extent3;
	typedef Extent<2, float> Size2;
	typedef Extent<3, float> Size3;
	typedef Extent<2, float> Size;
}