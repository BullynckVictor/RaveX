#pragma once
#include "Engine/Audio/AudioEngine.h"
#include "Engine/Graphics/GraphicsEngine.h"

namespace rv
{
	class Engine
	{
	public:
		Engine() = default;

		static Result Create(Engine& engine);
		void Release();

		AudioEngine audio;
		GraphicsEngine graphics;

	private:
	};
}