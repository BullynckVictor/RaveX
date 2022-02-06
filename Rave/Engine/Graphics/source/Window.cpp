#include "Engine/Graphics/Window.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Safety.h"
#include "Engine/Utility/Logger.h"

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
	title(strvalid(u"RaveEngine Window"))
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, WindowOptions options)
	:
	title(title),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, Flags<WindowOptions> options)
	:
	title(title),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, const Size& size, Flags<WindowOptions> options)
	:
	title(title),
	size(size),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, const Point& position, const Size& size, Flags<WindowOptions> options)
	:
	title(title),
	size(size),
	position(position),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title, const Size& size, Flags<WindowOptions> options)
	:
	title(std::move(title)),
	size(size),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title, const Point& position, const Size& size, Flags<WindowOptions> options)
	:
	title(std::move(title)),
	position(position),
	size(size),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title, Flags<WindowOptions> options)
	:
	title(std::move(title)),
	options(options)
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title, WindowOptions options)
	:
	title(title),
	options(options)
{
}

rv::Window::~Window()
{
}

rv::Result rv::Window::Create(Window& window, Descriptor&& descriptor)
{
	rv_result;

	if (!windowClass.Instance())
		rv_rif(Win32Class::Create(windowClass, u"Rave Window Class", StaticWindowSetupProc));

	window.title = std::move(descriptor.title);
	window.dpi = GetDpiForSystem();
	window.size = descriptor.size;
	window.position = descriptor.position;
	window.options = descriptor.options;
	window.styleEx = WS_EX_APPWINDOW;
	window.style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	if (descriptor.options.contain(RV_WINDOW_RESIZEABLE))
	{
		window.style |= WS_MAXIMIZEBOX;
		window.style |= WS_THICKFRAME;
	}

	Extent<2, int> size = CW_USEDEFAULT;
	Point position = CW_USEDEFAULT;
	
	RECT rect{};
	rect.left = safe_cast<long>(window.position.x);
	rect.top = safe_cast<long>(window.position.y);
	rect.right = safe_cast<long>(window.position.x + safe_cast<int>(window.size.width));
	rect.bottom = safe_cast<long>(window.position.y + safe_cast<int>(window.size.height));
	rif_check_last_msg(AdjustWindowRectExForDpi(&rect, window.style, false, window.styleEx, window.dpi), strvalid(u8"Unable to adjust window rectangle"));
	
	size.width = window.size.width ? (rect.right - rect.left) : CW_USEDEFAULT;
	size.height = window.size.height ? (rect.bottom - rect.top) : CW_USEDEFAULT;
	position.x = window.position.x ? rect.left : CW_USEDEFAULT;
	position.y = window.position.y ? rect.top : CW_USEDEFAULT;

	window.position *= window.dpi / 96;
	window.size *= window.dpi / 96;

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
		if (lastResult.failed())
			return lastResult;
	}
	return success;
}

bool rv::Window::Open() const
{
	return hwnd;
}

void rv::Window::Close()
{
	PostMessage(hwnd, WM_CLOSE, 0, 0);
}

rv::Result rv::Window::Resize(const rv::Size& size)
{
	return rv_check_condition(SetWindowPos(
		hwnd, 0, 
		position.x, position.y, 
		safe_cast<int>(size.width * dpi / 96), 
		safe_cast<int>(size.width * dpi / 96), 
		SWP_NOZORDER | SWP_NOMOVE | SWP_ASYNCWINDOWPOS
	));
}

rv::Result rv::Window::Resize(uint width, uint height)
{
	return Resize(rv::Size(width, height));
}

rv::Result rv::Window::SetPosition(const Point& position)
{
	return rv_check_condition(SetWindowPos(
		hwnd, 0, 
		position.x * dpi / 96, 
		position.y * dpi / 96, 
		size.width, size.width, 
		SWP_NOZORDER | SWP_NOSIZE | SWP_ASYNCWINDOWPOS
	));
}

rv::Result rv::Window::SetPosition(uint x, uint y)
{
	return SetPosition(Point(x, y));
}

rv::Result rv::Window::SetPositionResize(const Point& position, const rv::Size& size)
{
	return rv_check_condition(SetWindowPos(
		hwnd, 0,
		position.x * dpi / 96,
		position.y * dpi / 96,
		safe_cast<int>(size.width * dpi / 96),
		safe_cast<int>(size.width * dpi / 96),
		SWP_NOZORDER | SWP_ASYNCWINDOWPOS
	));
}

const rv::Point& rv::Window::Position() const
{
	return position;
}

const rv::Size& rv::Window::Size() const
{
	return size;
}

LRESULT rv::Window::StaticWindowSetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE)
	{
		CREATESTRUCT* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		if (create)
		{
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(StaticWindowProc));
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT rv::Window::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (window)
		return window->WindowProc(hwnd, msg, wParam, lParam);
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT rv::Window::WindowProc(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CLOSE:
		{
			SetResult(rv_check_condition_msg(DestroyWindow(hwnd), strvalid(u"Unable to destroy Window")));
		}
		return 0;
		case WM_DESTROY:
		{
			hwnd = nullptr;
		}
		return 0;
		case WM_PAINT:
//		case WM_MOVING:
//		case WM_SIZING:
		{
			SetResult(Draw());
		}
		return 0;

		case WM_DPICHANGED:
		{
			dpi = HIWORD(wParam);
			RECT* rect = reinterpret_cast<LPRECT>(lParam);
			SetResult(rv_assert_msg(rect, strvalid(u"WM_DPICHANGED has nullptr as lParam")));
			if (rect)
				SetResult(rv_check_condition(SetWindowPos(hwnd, 0, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, SWP_NOZORDER)));
		}
		return 0;
		case WM_SIZE:
		{
			if (wParam == SIZE_MINIMIZED)
				minimized = true;
			else
				minimized = false;

			size.width = LOWORD(lParam);
			size.height = HIWORD(lParam);
		}
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

rv::Result rv::Window::Draw()
{
	rv_result;

	return result;
}

void rv::Window::SetResult(Result result)
{
	if (result.severity() > lastResult.severity())
		lastResult = result;
}
