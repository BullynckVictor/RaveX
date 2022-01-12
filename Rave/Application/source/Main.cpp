#include <iostream>
#include "Engine/Rave.h"
#include <memory>

rv::EventLogger* logger;

struct ThreadEvent
{
	ThreadEvent() = default;
	ThreadEvent(int i) : i(i) {}
	std::thread::id id = std::this_thread::get_id();
	int i = 0;
};

void other_thread()
{
	for (int i = 0; i < 10000; ++i)
		logger->PostEvent(ThreadEvent(i));
}

rv::Result rv_main()
{
	std::cout << "Hello world!\n";
	std::cout << std::boolalpha;

	rv::EventLogger logger;
	::logger = &logger;

	std::thread thread1(other_thread);
	std::thread thread2(other_thread);

	rv::EventListener listener(logger);

	while (true)
	while (rv::Event event = listener.GetEvent())
	{
		if (event.IsType<ThreadEvent>())
			std::cout << "thread 0x" << std::hex << event.Get<ThreadEvent>().id << ": " << std::dec << event.Get<ThreadEvent>().i << '\n';
	}

	std::cin.ignore();
	thread1.join();
	thread2.join();
	return rv::failure;
}