#pragma once
#include "Engine/Audio/AudioEngine.h"

namespace rv
{
	class Engine
	{
	public:
		Engine() = default;

		static Result Create(Engine& engine);

		AudioEngine audio;

	private:
	};
}