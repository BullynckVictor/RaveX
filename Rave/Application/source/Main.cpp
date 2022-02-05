#include <iostream>
#include "Engine/Rave.h"

static constexpr bool failed = FAILED(ERROR_ACCOUNT_DISABLED);

rv::Result rv_main()
{
	rv_result;

	rv::Engine engine;
	rv_rif(rv::Engine::Create(engine));
	rv::EventListener threadListener(engine.graphics.thread);

	rv::Window& window = engine.graphics.CreateWindowRenderer();

	engine.graphics.thread.Await();

	while (window.Open())
	{
		while (rv::Event e = threadListener.GetEvent())
			if (e.IsType<rv::FailedResult>())
				return e.Get<rv::FailedResult>().result;
	}

	std::cin.ignore();
	return result;
}