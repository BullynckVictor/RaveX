#pragma once
#include "Engine/Core/Windows.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/String.h"

namespace rv
{
	class Win32Class
	{

	};

	class Window
	{
	public:
		Window() = default;

		static Result Create(Window& window);

	private:
		utf16_string title;

	};
}