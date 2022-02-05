#include "Engine/Graphics/Window.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Safety.h"

rv::Win32Class rv::Window::windowClass;

rv::Result rv::Win32Class::Create(Win32Class& window, utf16_cstring name, WNDPROC windowProc)
{
	window.windowClass.lpszClassName = name.c_str<wchar_t>();
	window.windowClass.lpfnWndProc = windowProc;
	window.windowClass.hInstance = GetModuleHandle(nullptr);
	return rv_check_last_msg(window.atom = RegisterClassEx(&window.windowClass), str16(strvalid("Unable to register win32 class \""), name, u'\"'));
}

const WNDCLASSEX& rv::Win32Class::Class() const
{
	return windowClass;
}

rv::utf16_cstring rv::Win32Class::Name() const
{
	return windowClass.lpszClassName;
}

HINSTANCE rv::Win32Class::Instance() const
{
	return windowClass.hInstance;
}

ATOM rv::Win32Class::Identifier() const
{
	return atom;
}

rv::WindowDescriptor::WindowDescriptor()
	:
	title(strvalid(u"RaveEngine Window")),
	options(detail::defaultWindowOptions)
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title)
	:
	title(title),
	options(detail::defaultWindowOptions)
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, const Extent<2, uint>& size, Flags<WindowOptions> options)
	:
	title(title),
	size(size),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options)
	:
	title(title),
	size(size),
	position(position),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title, const Extent<2, uint>& size, Flags<WindowOptions> options)
	:
	title(std::move(title)),
	size(size),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options)
	:
	title(std::move(title)),
	position(position),
	size(size),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title)
	:
	title(std::move(title)),
	options(detail::defaultWindowOptions)
{
}

rv::Window::~Window()
{
}

rv::Result rv::Window::Create(Window& window, Descriptor&& descriptor)
{
	rv_result;

	if (!windowClass.Instance())
		rv_rif(Win32Class::Create(windowClass, u"Rave Window Class", DefWindowProc));

	window.title = std::move(descriptor.title);
	window.size = descriptor.size;
	window.position = descriptor.position;
	window.options = descriptor.options;
	window.styleEx = WS_EX_APPWINDOW;
	window.style = WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;

	Extent<2, int> size = CW_USEDEFAULT;
	
	if (!window.options.contain(RV_WINDOW_SIZE_DEFAULT))
	{
		RECT rect {};
		rect.left = safe_cast<long>(window.position.x);
		rect.top = safe_cast<long>(window.position.y);
		rect.right = safe_cast<long>(window.position.x + window.size.width);
		rect.bottom = safe_cast<long>(window.position.y + window.size.height);
		rif_check_last_msg(AdjustWindowRectEx(&rect, window.style, false, window.styleEx), strvalid(u8"Unable to adjust window rectangle"));
		size.width = rect.right - rect.left;
		size.height = rect.bottom - rect.top;
	}

	Point position = window.position;
	if (window.options.contain(RV_WINDOW_POSITION_DEFAULT))
		position = CW_USEDEFAULT;

	rif_check_last_msg(window.hwnd = CreateWindowEx(
		window.styleEx,
		reinterpret_cast<const wchar_t*>(windowClass.Identifier()),
		window.title.c_str<wchar_t>(),
		window.style,
		position.x, position.y,
		size.width, size.height,
		nullptr, nullptr,
		windowClass.Instance(),
		&window
	), str16(strvalid(u"Unable to create window \""), window.title, u'\"'));

	if (window.hwnd)
		rif_check_last_msg(ShowWindow(window.hwnd, SW_SHOWNORMAL), str16(strvalid(u"Unable to show window \""), window.title, u'\"'));

	return result;
}

rv::Result rv::Window::Render()
{
	MSG msg;
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return success;
}

bool rv::Window::Open() const
{
	return hwnd;
}
