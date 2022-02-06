#include <iostream>
#include "Engine/Rave.h"

static constexpr bool failed = FAILED(ERROR_ACCOUNT_DISABLED);

rv::Result rv_main()
{
	rv_result;

	rv::Engine engine;
	rv_rif(rv::Engine::Create(engine));
	rv::EventListener threadListener(engine.graphics.thread);

	rv::Window& window = engine.graphics.CreateWindowRenderer("Hello from main.cpp!");

	engine.graphics.thread.Await();

	rv_log(rv::str16(L"Window size: (", window.Size().width, ", ", window.Size().height, ")"));
	while (window.Open())
	{
		engine.graphics.thread.RenderSingleThreaded();
		while (rv::Event e = threadListener.GetEvent())
			if (e.IsType<rv::FailedResult>())
				return e.Get<rv::FailedResult>().result;
	}

	return result;
}