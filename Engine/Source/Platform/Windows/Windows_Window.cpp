#include "ie_pch.h"

#include "Windows_Window.h"
#include "Insight/Core/Application.h"
#include "Insight/Events/Application_Event.h"
#include "Insight/Events/Mouse_Event.h"
#include "Insight/Events/Key_Event.h"
#include "Insight/Utilities/String_Helper.h"
#include "Insight/Core/Log.h"

#include <windowsx.h>

namespace Insight {


	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		m_Data.WindowTitle = props.Title;
		m_Data.WindowTitle_wide = StringHelper::StringToWide(m_Data.WindowTitle);
		m_Data.WindowClassName = props.Title + " Class";
		m_Data.WindowClassName_wide = StringHelper::StringToWide(m_Data.WindowClassName);
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
	}

	LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg) {
		case WM_NCCREATE:
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			WindowsWindow::WindowData* data = reinterpret_cast<WindowsWindow::WindowData*>(pCreate->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(data));

			return 1;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			WindowsWindow::WindowData data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			WindowCloseEvent event;
			data.EventCallback(event);
			return 0;
		}
		// Mouse Input
		case WM_MOUSEMOVE:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseMovedEvent event(LOWORD(lParam), HIWORD(lParam));
			data.EventCallback(event);
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseScrolledEvent event(0, GET_WHEEL_DELTA_WPARAM(wParam));
			data.EventCallback(event);
			return 0;
		}
		case WM_MOUSEHWHEEL:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseScrolledEvent event(GET_WHEEL_DELTA_WPARAM(wParam), 0);
			data.EventCallback(event);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonPressedEvent event(0);
			data.EventCallback(event);
			return 0;
		}
		case WM_LBUTTONUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonReleasedEvent event(0);
			data.EventCallback(event);
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonPressedEvent event(1);
			data.EventCallback(event);
			return 0;
		}
		case WM_RBUTTONUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonReleasedEvent event(1);
			data.EventCallback(event);
			return 0;
		}
		case WM_MBUTTONDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonPressedEvent event(2);
			data.EventCallback(event);
			return 0;
		}
		case WM_MBUTTONUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			MouseButtonReleasedEvent event(2);
			data.EventCallback(event);
			return 0;
		}
		// Keyboard Input
		case WM_CHAR:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			KeyTypedEvent event((char)wParam);
			data.EventCallback(event);
			return 0;
		}
		case WM_KEYDOWN:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
				WindowCloseEvent event;
				data.EventCallback(event);
				return 0;
			}


			if (wParam == VK_F11)
			{
				if (data.FullScreenEnabled)
				{
					data.FullScreenEnabled = false;
					WindowToggleFullScreenEvent event(false);
					data.EventCallback(event);
				}
				else
				{
					data.FullScreenEnabled = true;
					WindowToggleFullScreenEvent event(true);
					data.EventCallback(event);
				}

			}
			KeyPressedEvent event((char)wParam, 0);
			data.EventCallback(event);
			return 0;
		}
		case WM_KEYUP:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			KeyReleasedEvent event((char)wParam);
			data.EventCallback(event);
			return 0;
		}
		// Aplication Events
		case WM_COMPACTING:
		{
			IE_CORE_WARN("System memory is low!");
			return 0;
		}
		case WM_SIZE:
		{
			//IE_CORE_INFO("Window size has changed");

			//// CRASHES NO NOT RESIZE WINDOW
			//WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			//if (data.isFirstLaunch)
			//{
			//	data.isFirstLaunch = false;
			//	return 0;
			//}
			//RECT clientRect = {};
			//GetClientRect(hWnd, &clientRect);
			//WindowResizeEvent event(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
			//data.EventCallback(event);
			return 0;
		}
		case WM_INPUT:
		{
			WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			UINT dataSize;
			GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

			if (dataSize > 0)
			{
				std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
				if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
				{
					RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
					if (raw->header.dwType == RIM_TYPEMOUSE)
					{
						MouseRawMoveEvent event(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
						data.EventCallback(event);
					}
				}

			}
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		default:
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		}

	}

	bool WindowsWindow::Init(const WindowProps& props)
	{

		static bool raw_input_initialized = false;
		if (raw_input_initialized == false)
		{
			RAWINPUTDEVICE rid;

			rid.usUsagePage = 0x01; // Mouse
			rid.usUsage = 0x02;
			rid.dwFlags = 0;
			rid.hwndTarget = NULL;

			if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
			{
				IE_CORE_ERROR("Failed to register raw input devices. Error: {0}", GetLastError());
				exit(-1);
				// registration failed. Call GetLastError for the cause of the error
			}
			raw_input_initialized = true;
		}

		RegisterWindowClass();

		int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - m_Data.Width / 2;
		int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - m_Data.Height / 2;

		m_WindowRect.left = centerScreenX;
		m_WindowRect.top = centerScreenY;
		m_WindowRect.right = m_WindowRect.left + m_Data.Width;
		m_WindowRect.bottom = m_WindowRect.top + m_Data.Height;

		AdjustWindowRect(&m_WindowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

		m_WindowHandle = CreateWindowEx(
			0,										// Window Styles
			m_Data.WindowClassName_wide.c_str(),	// Window Class
			m_Data.WindowTitle_wide.c_str(),		// Window Title
			WS_OVERLAPPEDWINDOW,					// Window Style

			m_WindowRect.left,							// Start X
			m_WindowRect.top,							// Start Y
			m_WindowRect.right - m_WindowRect.left,		// Width
			m_WindowRect.bottom - m_WindowRect.top,		// Height

			NULL,					// Parent window
			NULL,					// Menu
			*m_WindowsAppInstance,	// Current Windows program application instance passed from WinMain
			&m_Data					// Additional application data
		);

		if (m_WindowHandle == NULL)
		{
			IE_ERROR("Unable to create Windows window.");
			IE_ERROR("    Error: {0}", GetLastError());
			return false;
		}

		m_pRendererContext = new Direct3D12Context(this);
		if (!m_pRendererContext->Init())
		{
			IE_CORE_FATAL(L"Failed to initialize graphics context");
			return false;
		}
		IE_CORE_TRACE("Renderer Initialized");

		ShowWindow(m_WindowHandle, m_nCmdShowArgs);
		SetForegroundWindow(m_WindowHandle);
		SetFocus(m_WindowHandle);
		SetWindowText(m_WindowHandle, m_Data.WindowTitle_wide.c_str());


		IE_CORE_TRACE("Window Initialized");
		return true;
	}

	void WindowsWindow::RegisterWindowClass()
	{
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0;
		wc.lpfnWndProc = WindowProcedure;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = *m_WindowsAppInstance;
		wc.hIcon = LoadIcon(0, IDI_WINLOGO);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.lpszMenuName = 0;
		wc.hbrBackground = 0;
		wc.lpszClassName = m_Data.WindowClassName_wide.c_str();

		RegisterClassEx(&wc);
		int error = GetLastError();
		if (error > 0)
		{
			IE_CORE_ERROR("An error occured while registering window class: {0} ", m_Data.WindowClassName);
			IE_CORE_ERROR("    Error: {1}", error);
		}
	}

	void WindowsWindow::Resize(UINT newWidth, UINT newHeight, bool isMinimized)
	{
		m_Data.Width = newWidth;
		m_Data.Height = newHeight;
		m_pRendererContext->SetWindowWidthAndHeight(newWidth, newHeight, isMinimized);
	}

	void WindowsWindow::ToggleFullScreen(bool enabled)
	{
		m_Data.FullScreenEnabled = enabled;
		m_pRendererContext->OnWindowFullScreen();
	}

	bool WindowsWindow::ProccessWindowMessages()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (PeekMessage(&msg,	// Where to store message (if one exists)
			m_WindowHandle,			// Handle to window we are checking messages for
			0,						// Minimum Filter Msg Value - We are not filterinf for specific messages but min and max could be used to do so
			0,						// Maximum Filter Msg Value
			PM_REMOVE))				// Remove mesage after captureing it via PeekMessage
		{
			if (msg.message == WM_QUIT)
				return false;
			TranslateMessage(&msg);		// Translate message from virtual key message into character messages
			DispatchMessage(&msg);		// Dispatch message to our WindowProc for this window
		}

		IE_ASSERT("Heap is currupted!", _CrtCheckMemory());
		return true;
	}

	WindowsWindow::~WindowsWindow()
	{
		IE_CORE_WARN("Destroying window: {0}", m_Data.WindowTitle);
		Shutdown();
	}

	void WindowsWindow::OnUpdate(const float& deltaTime)
	{
		ProccessWindowMessages();
		m_pRendererContext->OnUpdate(deltaTime);
	}

	void WindowsWindow::OnFramePreRender()
	{
		m_pRendererContext->OnPreFrameRender();
	}

	void WindowsWindow::OnRender()
	{
		m_pRendererContext->OnRender();
	}

	void WindowsWindow::ExecuteDraw()
	{
		m_pRendererContext->ExecuteDraw();
		m_pRendererContext->SwapBuffers();
	}

	void* WindowsWindow::GetNativeWindow() const
	{
		return m_WindowHandle;
	}

	void WindowsWindow::SetVSync(bool enabled)
	{
		IE_CORE_INFO("V-sync: " + enabled ? "enabled" : "disabled");
		m_Data.VSyncEnabled = enabled;
		m_pRendererContext->SetVSyncEnabled(m_Data.VSyncEnabled);
	}

	const bool& WindowsWindow::IsVsyncActive() const
	{
		return m_Data.VSyncEnabled;
	}

	const bool& WindowsWindow::IsFullScreenActive() const
	{
		return m_Data.FullScreenEnabled;
	}

	void WindowsWindow::Shutdown()
	{
		if (m_WindowHandle != NULL)
		{
			UnregisterClass(this->m_Data.WindowClassName_wide.c_str(), *m_WindowsAppInstance);
			DestroyWindow(m_WindowHandle);
		}

		delete m_pRendererContext;
	}

}

