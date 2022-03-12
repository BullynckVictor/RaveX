#pragma once
#include "Engine/Utility/Result.h"
#include "Engine/Graphics/GraphicsThread.h"
#include "Engine/Graphics/Window.h"
#include "Engine/Graphics/Instance.h"
#include "Engine/Graphics/DebugMessenger.h"
#include "Engine/Graphics/Device.h"

namespace rv
{
	class GraphicsEngine
	{
	public:
		GraphicsEngine() = default;

		static Result Create(GraphicsEngine& graphics);

		Window& CreateWindowRenderer(WindowDescriptor&& descriptor = {});
		Window& CreateWindowRenderer(const utf16_string& title, WindowOptions options);
		Window& CreateWindowRenderer(const utf16_string& title, Flags<WindowOptions> options = {});
		Window& CreateWindowRenderer(const utf16_string& title, const Extent<2, uint>& size, Flags<WindowOptions> options = {});
		Window& CreateWindowRenderer(const utf16_string& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options = {});
		Window& CreateWindowRenderer(utf16_string&& title, const Extent<2, uint>& size, Flags<WindowOptions> options = {});
		Window& CreateWindowRenderer(utf16_string&& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options = {});
		Window& CreateWindowRenderer(utf16_string&& title, Flags<WindowOptions> options = {});
		Window& CreateWindowRenderer(utf16_string&& title, WindowOptions options);

		Result CheckDebug();

		void Release();

	private:
		Instance instance;
		DebugMessenger debug;
		Device device;

	public:
		GraphicsThread thread;
	};
}