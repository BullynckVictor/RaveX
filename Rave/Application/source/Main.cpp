#include <iostream>
#include "Engine/Rave.h"

struct TestApp
{

};

rv::Result rv_main()
{
	rv_result;

	rv::Engine engine;
	rv_rif(rv::Engine::Create(engine));
	rv::EventListener threadListener(engine.graphics.thread);

	rv::Window& window = engine.graphics.CreateWindowRenderer("Hello from main.cpp!", rv::RV_WINDOW_RESIZEABLE);

	engine.graphics.thread.Await();

	window.SetTitle(u8"Hello from the main thread!");
	while (window.Open())
	{
		engine.graphics.thread.RenderSingleThreaded();
		while (rv::Event e = threadListener.GetEvent())
			if (e.IsType<rv::FailedResult>())
				return e.Get<rv::FailedResult>().result;
	}

	return result;
}