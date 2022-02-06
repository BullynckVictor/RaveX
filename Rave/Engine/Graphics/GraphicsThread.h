#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Utility/Queue.h"
#include "Engine/Utility/Any.h"
#include "Engine/Utility/Event.h"
#include "Engine/Graphics/Renderer.h"
#include <thread>
#include <mutex>
#include <deque>

namespace rv
{
	struct RendererInfo
	{
		RendererInfo() = default;
		template<Renderer R>
		void Set() { render = detail::make_render_function<R>; }
		template<Renderer R>
		static RendererInfo Make() { RendererInfo info; info.Set<R>(); return info; }

		RenderFunction render = nullptr;
	};

	struct FailedResult
	{
		FailedResult() = default;
		FailedResult(Result result) : result(result) {}

		Result result;
	};

	struct RendererCreateInfo
	{
		RendererCreateInfo() = default;
		template<Renderer R>
		void Set(R* renderer, typename R::Descriptor&& descriptor) { this->renderer = renderer; this->descriptor = std::move(Any(std::move(descriptor))); create = detail::make_create_function<R>; }
		template<Renderer R>
		void Set(R* renderer, const typename R::Descriptor& descriptor) { this->renderer = renderer; this->descriptor = std::move(Any(descriptor)); create = detail::make_create_function<R>; }
		template<Renderer R>
		static RendererCreateInfo Make(R* renderer, typename R::Descriptor&& descriptor) { RendererCreateInfo info; info.Set<R>(renderer, std::move(descriptor)); return info; }
		template<Renderer R>
		static RendererCreateInfo Make(R* renderer, const typename R::Descriptor& descriptor) { RendererCreateInfo info; info.Set<R>(renderer, descriptor); return info; }

		CreateFunction create = nullptr;
		Any descriptor;
		void* renderer = nullptr;
	};

	class GraphicsThread : public EventSource
	{
	public:
		GraphicsThread();
		~GraphicsThread();

		template<typename R>
		R* AddRenderer(typename R::Descriptor&& descriptor)
		{
			R* r = nullptr;
			if (MultiThreaded())
			{
				bool empty;
				{
					std::lock_guard guard1(createMutex);
					std::lock_guard guard2(queueMutex);
					empty = renderers.Empty();
					r = renderers.PushEntry<R>(RendererInfo::Make<R>(), R());
					createInfo.push_back(RendererCreateInfo::Make<R>(r, std::move(descriptor)));
				}
				if (empty)
					wakeUpSignal.notify_one();
			}
			else
			{
				r = renderers.PushEntry<R>(RendererInfo::Make<R>(), R());
				Result result = R::Create(*r, std::move(descriptor));
				if (result.failed())
					PostEvent(FailedResult(result));
			}
			return r;
		}
		template<typename R>
		R* AddRenderer(const typename R::Descriptor& descriptor)
		{
			R* r = nullptr;
			if (MultiThreaded())
			{
				bool empty;
				{
					std::lock_guard guard1(createMutex);
					std::lock_guard guard2(queueMutex);
					empty = renderers.Empty();
					r = renderers.PushEntry<R>(RendererInfo::Make<R>(), R());
					createInfo.push_back(RendererCreateInfo::Make<R>(r, descriptor));
				}
				if (empty)
					wakeUpSignal.notify_one();
			}
			else
			{
				r = renderers.PushEntry<R>(RendererInfo::Make<R>(), R());
				Result result = R::Create(*r, descriptor);
				if (result.failed())
					PostEvent(FailedResult(result));
			}
			return r;
		}

		bool FinishedCreating();
		void Await();

		static bool SingleThreaded();
		static bool MultiThreaded();

		void RenderSingleThreaded();

	private:
		void Task();
		static void StaticTask(GraphicsThread& thread);

	private:
		Queue<RendererInfo> renderers;
		std::deque<RendererCreateInfo> createInfo;
		std::mutex queueMutex;
		std::mutex createMutex;
		std::condition_variable wakeUpSignal;
		std::condition_variable finishedSignal;
		std::thread thread;
		bool shouldClose = false;
	};
}