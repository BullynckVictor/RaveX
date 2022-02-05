#pragma once
#include "Engine/Core/Windows.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/String.h"
#include "Engine/Utility/Vector.h"

namespace rv
{
	class Win32Class
	{
	public:
		Win32Class() = default;

		static Result Create(Win32Class& window, utf16_cstring name, WNDPROC windowProc);

		const WNDCLASSEX& Class() const;
		utf16_cstring Name() const;
		HINSTANCE Instance() const;
		ATOM Identifier() const;

	private:
		WNDCLASSEX windowClass { sizeof(WNDCLASSEX) };
		ATOM atom;
	};

	enum WindowOptions
	{
		RV_WINDOW_POSITION_DEFAULT	= make_flag<WindowOptions>(0),
		RV_WINDOW_SIZE_DEFAULT		= make_flag<WindowOptions>(1),
		RV_WINDOW_RESIZEABLE		= make_flag<WindowOptions>(2),
	};

	namespace detail
	{
		static constexpr Flags<WindowOptions> defaultWindowOptions = make_flags<WindowOptions>(RV_WINDOW_POSITION_DEFAULT, RV_WINDOW_SIZE_DEFAULT);
	}

	struct WindowDescriptor
	{
		WindowDescriptor();
		WindowDescriptor(const utf16_string& title);
		WindowDescriptor(const utf16_string& title, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(RV_WINDOW_SIZE_DEFAULT));
		WindowDescriptor(const utf16_string& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(make_flags<WindowOptions>(RV_WINDOW_POSITION_DEFAULT, RV_WINDOW_SIZE_DEFAULT)));
		WindowDescriptor(utf16_string&& title, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(RV_WINDOW_SIZE_DEFAULT));
		WindowDescriptor(utf16_string&& title, const Vector<2, uint>& position, const Extent<2, uint>& size, Flags<WindowOptions> options = detail::defaultWindowOptions.without(make_flags<WindowOptions>(RV_WINDOW_POSITION_DEFAULT, RV_WINDOW_SIZE_DEFAULT)));
		WindowDescriptor(utf16_string&& title);

		utf16_string title;
		Vector<2, uint> size;
		Extent<2, uint> position;
		Flags<WindowOptions> options;
	};

	class Window
	{
	public:
		using Descriptor = WindowDescriptor;

		Window() = default;
		~Window();

		static Result Create(Window& window, Descriptor&& descriptor = {});

		Result Render();

		bool Open() const;

	private:
		static Win32Class windowClass;

		utf16_string title;
		Extent<2, uint> size;
		Vector<2, uint> position;
		DWORD styleEx = 0;
		DWORD style = 0;
		Flags<WindowOptions> options;
		HWND hwnd = 0;
	};
}