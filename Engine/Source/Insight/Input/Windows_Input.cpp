#include <ie_pch.h>
#include "Windows_Input.h"

#include "Insight/Core/Application.h"
#include "Platform/Windows/Windows_Window.h"


namespace Insight {

	Input* Input::s_Instance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int keycode)
	{
		auto state = m_InputManager.GetKeyboardBuffer().GetKeyState(keycode);
		return state == InputBuffer::PRESSED;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int button)
	{
		auto state = m_InputManager.GetMouseBuffer().GetButtonState(button);
		return state == InputBuffer::PRESSED;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		double xPos, yPos;
		m_InputManager.GetMouseBuffer().GetMousePosition(xPos, yPos);
		return { (float)xPos, (float)yPos };
	}

	std::pair<int, int> WindowsInput::GetRawMousePositionImpl()
	{
		int xPos, yPos;
		m_InputManager.GetMouseBuffer().GetRawMousePosition(xPos, yPos);
		m_InputManager.GetMouseBuffer().OnRawMousePositionMoved(0, 0);
		return { (int)xPos, (int)yPos };
	}

	float WindowsInput::GetMouseXImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return (float)x;
	}

	float WindowsInput::GetMouseYImpl()
	{
		auto [x, y] = GetMousePositionImpl();
		return (float)y;
	}

	bool WindowsInput::IsMouseScrolledImpl()
	{
		auto state = m_InputManager.GetMouseBuffer().GetScrollWheelState();
		return state == InputBuffer::SCROLL_UP || state == InputBuffer::SCROLL_DOWN;
	}

	std::pair<float, float> WindowsInput::GetMouseScrollOffsetImpl()
	{
		float xOffset, yOffset;
		m_InputManager.GetMouseBuffer().GetMouseScrollOffset(xOffset, yOffset);
		return { xOffset, yOffset };
	}

}