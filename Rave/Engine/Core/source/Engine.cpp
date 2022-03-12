#include "Engine/Core/Engine.h"
#include "Engine/Utility/Error.h"

rv::Result rv::Engine::Create(Engine& engine)
{
	rv_result;

	rv_rif(AudioEngine::Create(engine.audio));
	rv_rif(GraphicsEngine::Create(engine.graphics));

	return result;
}

void rv::Engine::Release()
{
	graphics.Release();
}
