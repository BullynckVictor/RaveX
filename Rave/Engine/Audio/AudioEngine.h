#pragma once
#include "Engine/Utility/Result.h"

namespace rv
{
	class AudioEngine
	{
	public:
		AudioEngine() = default;

		static Result Create(AudioEngine& engine);

	private:
	};
}