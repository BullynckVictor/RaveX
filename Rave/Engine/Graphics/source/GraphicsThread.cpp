#include "Engine/Graphics/GraphicsThread.h"
#include "Engine/Utility/Error.h"
#include <functional>

rv::GraphicsThread::GraphicsThread()
	:
	thread(MultiThreaded() ? std::thread(StaticTask, std::ref(*this)) : std::thread())
{
}

rv::GraphicsThread::~GraphicsThread()
{
	shouldClose = true;
	if (thread.joinable())
	{
		wakeUpSignal.notify_one();
		thread.join();
	}
}

bool rv::GraphicsThread::FinishedCreating()
{
	std::lock_guard guard(createMutex);
	return createInfo.empty();
}

void rv::GraphicsThread::Await()
{
	if (SingleThreaded() || FinishedCreating())
		return;
	std::mutex mutex;
	std::unique_lock lock(mutex);
	finishedSignal.wait(lock);
}

bool rv::GraphicsThread::SingleThreaded()
{
	return std::thread::hardware_concurrency() <= 1;
}

bool rv::GraphicsThread::MultiThreaded()
{
	return !SingleThreaded();
}

void rv::GraphicsThread::RenderSingleThreaded()
{
	if (SingleThreaded())
	{
		Result result;
		for (auto* header = renderers.PeekHeader(); header; header = header->next)
		{
			result = header->info.render(header->data());
			if (result.failed())
				PostEvent(FailedResult(result));
		}
	}
}

void rv::GraphicsThread::Task()
{
	std::mutex mutex;
	std::unique_lock lock(mutex);
	while (true)
	{
		std::unique_lock queueLock(queueMutex);
		bool empty = renderers.Empty();
		queueLock.unlock();
		if (empty)
			wakeUpSignal.wait(lock);
		if (shouldClose)
		{
			queueLock.lock();
			renderers.Clear();
			return;
		}
		
		while (true)
		{
			Result result;
			bool must_create;
			{
				std::lock_guard guard(createMutex);
				must_create = !createInfo.empty();
				for (auto& create : createInfo)
				{
					result = create.create(create.renderer, create.descriptor.Data());
					if (result.failed())
						PostEvent(FailedResult(result));
				}
				createInfo.clear();
			}
			if (must_create)
				finishedSignal.notify_all();

			queueLock.lock();
			for (auto* header = renderers.PeekHeader(); header; header = header->next)
			{
				result = header->info.render(header->data());
				if (result.failed())
					PostEvent(FailedResult(result));
			}
			if (shouldClose)
			{
				renderers.Clear();
				return;
			}
			queueLock.unlock();
		}
	}
}

void rv::GraphicsThread::StaticTask(GraphicsThread& thread)
{
	return thread.Task();
}
