#pragma once
#include "Engine/Utility/Types.h"
#include "Engine/Utility/Flags.h"
#include <typeinfo>

namespace rv
{
	struct EmptyInfo {};

	template<typename I = EmptyInfo>
	class Queue
	{
	public:
		Queue() = default;
		Queue(const Queue&) = delete;
		Queue(Queue&& rhs) noexcept
			:
			first(rhs.first),
			last(rhs.last)
		{
			rhs.first = nullptr;
			rhs.last = nullptr;
		}
		~Queue()
		{
			Clear();
		}

		Queue& operator= (const Queue&) = delete;
		Queue& operator= (Queue&& rhs) noexcept
		{
			Clear();
			first = rhs.first;
			last = rhs.last;
			rhs.first = nullptr;
			rhs.last = nullptr;
			return *this;
		}

		template<typename D>
		D* PushEntry(const I& info, const D& data)
		{
			return PushHeader(&(new Entry<D>(info, data))->header)->data<D>();
		}
		template<typename D>
		D* PushEntry(const I& info, D&& data)
		{
			return PushHeader(&(new Entry<D>(info, std::move(data)))->header)->data<D>();
		}
		void* PushEntry(const I& info)
		{
			Header* header = new Header();
			header->info = info;
			header->type = typeid(void).hash_code();
			PushHeader(header);
			return header;
		}
		void* PushEntry(I&& info)
		{
			Header* header = new Header();
			header->info = std::move(info);
			header->type = typeid(void).hash_code();
			PushHeader(header);
			return header;
		}

		void Clear()
		{
			if (first)
				delete first;
			first = nullptr;
		}

	private:
		template<typename I>
		static void make_destructor(void* info)
		{
			reinterpret_cast<I*>(info)->~I();
		}

	public:
		typedef void(*Destructor)(void*);
		struct Header
		{
			Header() = default;
			~Header()
			{
				if (destructor)
					destructor(data());
				if (next)
					delete next;
			}

			template<typename T = void>
			T* data() { return reinterpret_cast<T*>(reinterpret_cast<byte*>(this) + dataOffset); }
			template<typename T = void>
			const T* data() const { return reinterpret_cast<const T*>(reinterpret_cast<const byte*>(this) + dataOffset); }

			Header* next = nullptr;
			Header* prev = nullptr;
			size_t dataOffset = 0;
			Destructor destructor = nullptr;
			size_t type = 0;
			I info;
		};

	private:
		template<typename D>
		struct Entry
		{
			Entry() = default;
			Entry(const I& info, const D& data)
				:
				data(data)
			{
				header.info = info;
				header.type = typeid(D).hash_code();
				header.dataOffset = offsetof(Entry<D>, data) - offsetof(Entry<D>, header);
				if constexpr (std::is_class_v<D>)
					header.destructor = make_destructor<D>;
			}
			Entry(const I& info, D&& data)
				:
				data(std::move(data))
			{
				header.info = info;
				header.type = typeid(D).hash_code();
				header.dataOffset = offsetof(Entry<D>, data) - offsetof(Entry<D>, header);
				if constexpr (std::is_class_v<D>)
					header.destructor = make_destructor<D>;
			}

			Header header;
			D data;
		};

	private:
		Header* PushHeader(Header* header)
		{
			Header* prevLast = last;
			header->prev = prevLast;
			last = header;

			if (!first)
				first = header;
			else
				prevLast->next = header;

			return header;
		}

	public:
		Header* PeekHeader()
		{
			return first;
		}

		Header* GetHeader()
		{
			if (first)
			{
				Header* f = first;
				first = first->next;
				if (!first) // no headers
					last = nullptr;
				else
				{
					first->prev = nullptr;
					if (!first->next) // the only header
						last = first;
				}
				f->next = nullptr;
				return f;
			}
			return nullptr;
		}

		template<typename C, typename... Args>
		Header* GetHeader(const C& checker, const Args&... args)
		{
			for (Header* header = first; header; header = header->next)
			{
				if (checker(header, args...))
				{
					if (header->prev)
						header->prev->next = header->next;
					else
						//this header is the first
						first = header->next;

					if (header->next)
						header->next->prev = header->prev;
					else
						//this header is the last
						last = header->prev;

					return header;
				}
			}
			return nullptr;
		}

		bool Empty() const
		{
			return !first;
		}

	private:
		Header* first = nullptr;
		Header* last = nullptr;
	};
}