#pragma once
#include "Engine/Utility/Types.h"
#include <typeinfo>

namespace rv
{
	class Any
	{
	public:
		Any() : m_data(nullptr) {}
		Any(const Any&) = delete;
		Any(Any&& rhs) noexcept : m_data(rhs.m_data) { rhs.m_data = nullptr; }
		~Any() { Clear(); }

		template<typename T> Any(const T& value) : m_data(&(new Entry<T>(value))->header) {}
		template<typename T> Any(T&& value) : m_data(&(new Entry<T>(std::move(value)))->header) {}

		Any& operator=(const Any&) = delete;
		Any& operator=(Any&& rhs) noexcept { Clear(); m_data = rhs.m_data; rhs.m_data = nullptr; return *this; }

		operator bool() const { return Empty(); }
		bool Empty() const { return m_data; }

		template<typename T>
		bool IsType() const { return m_data ? (m_data->type == typeid(T).hash_code()) : false; }

		template<typename T>
		T& Get() { return *m_data->data<T>(); }
		template<typename T>
		const T& Get() const { return *m_data->data<T>(); }

		void* Data() { return m_data->data(); }
		const void* Data() const { return m_data->data(); }

		void Clear() { if (m_data) delete m_data; }

	private:
		typedef void(*Destructor)(void*);
		template<typename T>
		static void make_destructor(void* object)
		{
			if (object)
				reinterpret_cast<T*>(object)->~T();
		}

		struct Header
		{
			Header() = default;
			~Header()
			{
				if (destructor)
					destructor(data());
			}

			template<typename T = void>
			T* data() { return reinterpret_cast<T*>(reinterpret_cast<byte*>(this) + data_offset); }
			template<typename T = void>
			const T* data() const { return reinterpret_cast<const T*>(reinterpret_cast<const byte*>(this) + data_offset); }

			Destructor destructor = nullptr;
			size_t data_offset = 0;
			size_t type = 0;
		};
		template<typename T>
		struct Entry
		{
			Entry() = default;
			Entry(const T& value) : data(value) 
			{
				header.type = typeid(T).hash_code();
				header.data_offset = offsetof(Entry<T>, data) - offsetof(Entry<T>, header);
				if constexpr (std::is_class_v<T>)
					header.destructor = make_destructor<T>;
			}
			Entry(T&& value) : data(std::move(value))
			{
				header.type = typeid(T).hash_code();
				header.data_offset = offsetof(Entry<T>, data) - offsetof(Entry<T>, header);
				if constexpr (std::is_class_v<T>)
					header.destructor = make_destructor<T>;
			}
			Header header;
			T data;
		};

		Header* m_data;
	};
}