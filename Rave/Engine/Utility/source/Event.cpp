#include "Engine/Utility/Event.h"

rv::EventListener::EventListener(EventLogger& logger)
{
	Listen(logger);
}

rv::EventListener::EventListener(EventSource& source)
{
	Listen(source);
}

void rv::EventListener::Listen(EventLogger& logger)
{
	events = std::make_shared<EventQueue>();
	std::lock_guard guard(logger.mutex);
	logger.listeners.push_back(events);
}

void rv::EventListener::Listen(EventSource& source)
{
	Listen(source.logger);
}

void rv::EventListener::StopListening()
{
	events.reset();
}

rv::Event rv::EventListener::GetEvent()
{
	std::lock_guard guard(events->mutex);
	return events->queue.GetHeader();
}

rv::Event::Event(Queue<EmptyInfo>::Header* header)
	:
	header(header)
{
	if (header)
	{
		header->prev = nullptr;
		header->next = nullptr;
	}
}

rv::Event::Event(Event&& rhs) noexcept
	:
	header(header)
{
	rhs.header = nullptr;
}

rv::Event::~Event()
{
	Clear();
}

rv::Event& rv::Event::operator=(Event&& rhs) noexcept
{
	Clear();
	header = rhs.header;
	rhs.header = nullptr;
	return *this;
}

rv::Event::operator bool() const
{
	return !Empty();
}

bool rv::Event::Empty() const
{
	return !header;
}

void rv::Event::Clear()
{
	if (header)
		delete header;
}
