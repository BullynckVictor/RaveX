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

	const auto& size = window.Size();
	window.SetTitle(rv::str16(size.width, " x ", size.height));
	auto prev = size;
	while (window.Open())
	{
		engine.graphics.thread.RenderSingleThreaded();
		if (size != prev)
		{
			prev = size;
			window.SetTitle(rv::str16(size.width, " x ", size.height));
		}
		while (rv::Event e = threadListener.GetEvent())
			if (e.IsType<rv::FailedResult>())
				return e.Get<rv::FailedResult>().result;
		if constexpr (rv::DebugMessenger::enabled)
			rv_rif(engine.graphics.CheckDebug());
	}

	engine.graphics.thread.Await();
	return result;
}