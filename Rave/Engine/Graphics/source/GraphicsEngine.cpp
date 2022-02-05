#include "Engine/Graphics/GraphicsEngine.h"

rv::Result rv::GraphicsEngine::Create(GraphicsEngine& graphics)
{
	return success;
}

rv::Window& rv::GraphicsEngine::CreateWindowRenderer(WindowDescriptor&& descriptor)
{
	return *thread.AddRenderer<Window>(std::move(descriptor));
}

rv::Window& rv::GraphicsEngine::CreateWindowRenderer(const utf16_string& title) { return CreateWindowRenderer(WindowDescriptor(title)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(const utf16_string& title, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(title, size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(const utf16_string& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(title, position, size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(utf16_string&& title, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(std::move(title), size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(utf16_string&& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options) { return CreateWindowRenderer(WindowDescriptor(std::move(title), position, size, options)); }
rv::Window& rv::GraphicsEngine::CreateWindowRenderer(utf16_string&& title) { return CreateWindowRenderer(WindowDescriptor(std::move(title))); }
