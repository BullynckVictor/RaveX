#include "Engine/Graphics/Window.h"
#include "Engine/Utility/Error.h"
#include "Engine/Utility/Safety.h"
#include "Engine/Utility/Logger.h"

rv::Win32Class rv::Window::windowClass;

rv::Result rv::Win32Class::Create(Win32Class& window, utf16_cstring name, WNDPROC windowProc)
{
	rv_result;
	window.windowClass.lpszClassName = name.c_str<wchar_t>();
	window.windowClass.lpfnWndProc = windowProc;
	window.windowClass.hInstance = GetModuleHandle(nullptr);
	window.windowClass.hCursor = static_cast<HCURSOR>(LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED));
	rif_assert_msg(window.windowClass.hCursor, strvalid(u"Unable to load cursor"));
	return rv_check_last_msg(window.atom = RegisterClassEx(&window.windowClass), str16(strvalid(u"Unable to register win32 class \""), name, u'\"'));
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
	options(options),
	position(std::numeric_limits<int>::min())
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, Flags<WindowOptions> options)
	:
	title(title),
	options(options),
	position(std::numeric_limits<int>::min())
{
}

rv::WindowDescriptor::WindowDescriptor(const utf16_string& title, const Size& size, Flags<WindowOptions> options)
	:
	title(title),
	size(size),
	options(options),
	position(std::numeric_limits<int>::min())
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
	options(options),
	position(std::numeric_limits<int>::min())
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
	options(options),
	position(std::numeric_limits<int>::min())
{
}

rv::WindowDescriptor::WindowDescriptor(utf16_string&& title, WindowOptions options)
	:
	title(title),
	options(options),
	position(std::numeric_limits<int>::min())
{
}

rv::Window::Window()
	:
	mutex(new std::mutex())
{
}

rv::Window::~Window()
{
	Close();
}

rv::Result rv::Window::Create(Window& window, const Device& device, Descriptor&& descriptor)
{
	rv_result;

	if (!windowClass.Instance())
		rv_rif(Win32Class::Create(windowClass, u"Rave Window Class", StaticWindowSetupProc));

	window.title = std::move(descriptor.title);
	window.dpi = GetDpiForSystem();
	window.options = descriptor.options;
	window.styleEx = WS_EX_APPWINDOW;
	window.style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
	window.device = &device;
	if (descriptor.options.contain(RV_WINDOW_RESIZEABLE))
	{
		window.style |= WS_MAXIMIZEBOX;
		window.style |= WS_THICKFRAME;
	}

	Point position;
	position.x = window.position.x == std::numeric_limits<int>::min() ? 0 : window.position.x;
	position.y = window.position.y == std::numeric_limits<int>::min() ? 0 : window.position.y;

	RECT rect{};
	rect.left = safe_cast<long>(position.x);
	rect.top = safe_cast<long>(position.y);
	rect.right = safe_cast<long>(position.x + descriptor.size.x);
	rect.bottom = safe_cast<long>(position.y + descriptor.size.y);
	rif_check_last_msg(AdjustWindowRectExForDpi(&rect, window.style, false, window.styleEx, 120), strvalid(u8"Unable to adjust window rectangle"));
	
	window.position.x = descriptor.position.x == std::numeric_limits<int>::min() ? CW_USEDEFAULT : rect.left;
	window.position.y = descriptor.position.y == std::numeric_limits<int>::min() ? CW_USEDEFAULT : rect.top;
	window.size.width = descriptor.size.x ? safe_cast<uint>(rect.right - rect.left) : CW_USEDEFAULT;
	window.size.height = descriptor.size.y ? safe_cast<uint>(rect.bottom - rect.top) : CW_USEDEFAULT;

	rif_check_last_msg(window.hwnd = CreateWindowEx(
		window.styleEx,
		reinterpret_cast<const wchar_t*>(windowClass.Identifier()),
		window.title.c_str<wchar_t>(),
		window.style,
		window.position.x, window.position.y,
		(int)window.size.width, (int)window.size.height,
		nullptr, nullptr,
		windowClass.Instance(),
		&window
	), str16(strvalid(u"Unable to create window \""), window.title, u'\"'));

	if (window.hwnd)
		rif_check_last_msg(ShowWindow(window.hwnd, SW_SHOWNORMAL), str16(strvalid(u"Unable to show window \""), window.title, u'\"'));

	rv_log(str16(strvalid(u8"Created window \""), window.title, u'\"'));

	return result;
}

rv::Result rv::Window::Render()
{
	MSG msg;
	drawn = false;
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (lastResult.failed())
			return lastResult;
	}
	{
		std::lock_guard guard(*mutex);
		if (updatedTitle)
		{
			title = std::move(newTitle);
			SetWindowText(hwnd, title.c_str<wchar_t>());
		}
		updatedTitle = false;
	}
	if (drawn)
		return success;
	return Draw();
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

void rv::Window::SetTitle(const utf16_string& title)
{
	std::lock_guard guard(*mutex);
	newTitle = title;
	updatedTitle = true;
}

void rv::Window::SetTitle(utf16_string&& title)
{
	std::lock_guard guard(*mutex);
	newTitle = std::move(title);
	updatedTitle = true;
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
		case WM_MOVING:
		case WM_SIZING:
		{
			SetResult(Draw());
		}
		return 0;

		case WM_DPICHANGED:
		{
			dpi = HIWORD(wParam);
			RECT* rect = reinterpret_cast<LPRECT>(lParam);
			if constexpr (build.debug)
				SetResult(rv_check_condition_msg(rect, strvalid(u"WM_DPICHANGED has nullptr as lParam")));
			if (rect)
				SetResult(rv_check_condition(SetWindowPos(hwnd, 0, rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top, SWP_NOZORDER)));
		}
		return 0;
		case WM_SIZE:
		{
			size.width = LOWORD(lParam);
			size.height = HIWORD(lParam);

			if (wParam == SIZE_MINIMIZED)
			{
				minimized = true;
			}
			else
			{
				minimized = false;
				SetResult(Swapchain::Create(swap, *device, *this));
			}
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
