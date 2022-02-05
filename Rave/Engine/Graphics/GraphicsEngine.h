#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/GraphicsThread.h"
#include "Engine/Graphics/Window.h"

namespace rv
{
	class GraphicsEngine
	{
	public:
		GraphicsEngine() = default;

		static Result Create(GraphicsEngine& graphics);

		Window& CreateWindowRenderer(WindowDescriptor&& descriptor = {});
		Window& CreateWindowRenderer(const utf16_string& title);
		Window& CreateWindowRenderer(const utf16_string& title, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(RV_WINDOW_SIZE_DEFAULT));
		Window& CreateWindowRenderer(const utf16_string& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(make_flags<WindowOptions>(RV_WINDOW_POSITION_DEFAULT, RV_WINDOW_SIZE_DEFAULT)));
		Window& CreateWindowRenderer(utf16_string&& title, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(RV_WINDOW_SIZE_DEFAULT));
		Window& CreateWindowRenderer(utf16_string&& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(make_flags<WindowOptions>(RV_WINDOW_POSITION_DEFAULT, RV_WINDOW_SIZE_DEFAULT)));
		Window& CreateWindowRenderer(utf16_string&& title);

	public:
		GraphicsThread thread;
	};
}