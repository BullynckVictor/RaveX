#include <iostream>
#include "Engine/Rave.h"


rv::Result rv_main()
{
	rv_result;

	rv::Engine engine;
	rv_rif(rv::Engine::Create(engine));
	rv::EventListener threadListener(engine.graphics.thread);

	rv::Window& window = engine.graphics.CreateWindowRenderer("Rave Window", rv::Size(800, 500), rv::RV_WINDOW_RESIZEABLE);

	engine.graphics.thread.Await();

	window.SetTitle(u"Hello from the main thread!");
	while (window.Open())
	{
		engine.graphics.thread.RenderSingleThreaded();
		while (rv::Event e = threadListener.GetEvent())
			if (e.IsType<rv::FailedResult>())
				return e.Get<rv::FailedResult>().result;
		if constexpr (rv::DebugMessenger::enabled)
			rv_rif(engine.graphics.CheckDebug());
	}

	return result;
}