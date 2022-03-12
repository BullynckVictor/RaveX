#include "Engine/Graphics/GraphicsEngine.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Logger.h"

rv::Result rv::GraphicsEngine::Create(GraphicsEngine& graphics)
{
	rv_result;
	rv_rif(Instance::Create(graphics.instance));
	if constexpr (graphics.debug.enabled)
		rv_rif(DebugMessenger::Create(graphics.debug, graphics.instance));
	rv_rif(Device::Create(graphics.device, graphics.instance));
	return result;
}

rv::Window& rv::GraphicsEngine::CreateWindowRenderer(WindowDescriptor&& descriptor)
{
	return *thread.AddRenderer<Window>(device, std::move(descriptor));
}

rv::Window& rv::GraphicsEngine::CreateWindowRenderer(const utf16_string& title, WindowOptions options) { return CreateWindowRenderer(title, Flags<WindowOptions>(options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(const utf16_string& title, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(title, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(const utf16_string& title, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(title, size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(const utf16_string& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(title, position, size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(utf16_string&& title, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(std::move(title), size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(utf16_string&& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(std::move(title), position, size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(utf16_string&& title, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(std::move(title), options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(utf16_string&& title, WindowOptions options) { return CreateWindowRenderer(std::move(title), Flags<WindowOptions>(options)); }

rv::Result rv::GraphicsEngine::CheckDebug()
{
	return debug.CheckResult();
}

void rv::GraphicsEngine::Release()
{
	device.Release();
	debug.Release();
	instance.Release();
}
