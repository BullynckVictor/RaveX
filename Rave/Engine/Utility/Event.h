#pragma once
#include "Engine/Utility/Types.h"
#include "Engine/Utility/Queue.h"
#include "Engine/Utility/TimeStamp.h"
#include "Engine/Utility/Result.h"
#include <vector>
#include <memory>
#include <mutex>

namespace rv
{
	struct EventQueue
	{
		EventQueue() = default;
		Queue<EmptyInfo> queue;
		std::mutex mutex;
	};

	class EventLogger
	{
	public:
		template<typename E>
		void PostEvent(const E& event)
		{
			std::lock_guard global_guard(mutex);
			for (auto& listener : listeners)
			{
				if (listener.use_count() == 1)
				{
					listener.reset();
					continue;
				}
				std::lock_guard guard(listener->mutex);
				listener->queue.PushEntry(EmptyInfo(), event);
			}
		}
		template<typename E>
		void PostEvent(E&& event)
		{
			std::lock_guard global_guard(mutex);			
			for (auto& listener : listeners)
			{
				if (listener.use_count() == 1)
				{
					listener.reset();
					continue;
				}
				std::lock_guard guard(listener->mutex);
				listener->queue.PushEntry(EmptyInfo(), std::move(event));
			}
		}

	private:
		std::vector<std::shared_ptr<EventQueue>> listeners;
		std::mutex mutex;
		friend class EventListener;
	};

	class EventSource
	{
	protected:
		template<typename E>
		void PostEvent(const E& event)
		{
			logger.PostEvent(event);
		}
		template<typename E>
		void PostEvent(E&& event)
		{
			logger.PostEvent(std::move(event));
		}

	private:
		EventLogger logger;
		friend class EventListener;
	};

	struct Event
	{
	public:
		Event() = default;
		Event(Queue<EmptyInfo>::Header* header);
		Event(const Event&) = delete;
		Event(Event&& rhs) noexcept;
		~Event();

		Event& operator= (const Event&) = delete;
		Event& operator= (Event&& rhs) noexcept;

		operator bool() const;
		bool Empty() const;

		template<typename E>
		bool IsType() const { return header ? (header->type == typeid(E).hash_code()) : false; }

		template<typename E>
		E& Get() { return *header->data<E>(); }
		template<typename E>
		const E& Get() const { return *header->data<E>(); }

		void Clear();

	private:
		Queue<EmptyInfo>::Header* header = nullptr;
	};

	class EventListener
	{
	public:
		EventListener() = default;
		EventListener(EventLogger& logger);
		EventListener(EventSource& source);

		void Listen(EventLogger& logger);
		void Listen(EventSource& source);
		void StopListening();

		Event GetEvent();

	private:
		std::shared_ptr<EventQueue> events;
	};
}