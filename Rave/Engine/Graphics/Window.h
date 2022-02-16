#pragma once
#include "Engine/Core/Windows.h"
#include "Engine/Utility/Result.h"
#include "Engine/Utility/String.h"
#include "Engine/Utility/Vector.h"
#include <map>
#include <mutex>

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
		RV_WINDOW_RESIZEABLE = make_flag<WindowOptions>(0),
	};

	struct WindowDescriptor
	{
		WindowDescriptor();
		WindowDescriptor(const utf16_string& title, WindowOptions options);
		WindowDescriptor(const utf16_string& title, Flags<WindowOptions> options = {});
		WindowDescriptor(const utf16_string& title, const Size& size, Flags<WindowOptions> options = {});
		WindowDescriptor(const utf16_string& title, const Point& position, const Size& size, Flags<WindowOptions> options = {});
		WindowDescriptor(utf16_string&& title, const Size& size, Flags<WindowOptions> options = {});
		WindowDescriptor(utf16_string&& title, const Point& position, const Size& size, Flags<WindowOptions> options = {});
		WindowDescriptor(utf16_string&& title, Flags<WindowOptions> options = {});
		WindowDescriptor(utf16_string&& title, WindowOptions options);

		utf16_string title;
		Vector<2, uint> size;
		Point position;
		Flags<WindowOptions> options;
	};

	class Window
	{
	public:
		using Descriptor = WindowDescriptor;

		Window();
		Window(const Window&) = delete;
		Window(Window&&) noexcept = default;
		~Window();

		Window& operator= (const Window&) = delete;
		Window& operator= (Window&&) noexcept = default;

		static Result Create(Window& window, Descriptor&& descriptor = {});

		Result Render();

		bool Open() const;
		void Close();

		Result Resize(const Size& size);
		Result Resize(uint width, uint height);
		Result SetPosition(const Point& position);
		Result SetPosition(uint x, uint y);
		Result SetPositionResize(const Point& position, const Size& size);

		void SetTitle(const utf16_string& title);
		void SetTitle(utf16_string&& title);

		const Point& Position() const;
		const Size& Size() const;

	private:
		static Win32Class windowClass;
		static LRESULT CALLBACK StaticWindowSetupProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		Result Draw();

		void SetResult(Result result);

	private:
		utf16_string title;
		rv::Size size;
		Point position;
		DWORD styleEx = 0;
		DWORD style = 0;
		Flags<WindowOptions> options;
		HWND hwnd = 0;
		Result lastResult;
		bool minimized = false;
		uint dpi = 96;
		utf16_string newTitle;
		bool updatedTitle = false;
		std::unique_ptr<std::mutex> mutex;
		bool drawn = false;
	};
}