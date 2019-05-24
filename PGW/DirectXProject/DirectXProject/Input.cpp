#include "stdafx.h"
#include "Input.h"

IMPLEMENT_SINGLETON(CInput)


CInput::CInput(void)
	: m_pInputSDK(NULL),
	  m_pKeyBoard(NULL),
	  m_pMouse(NULL)
{
	ZeroMemory(m_byKeyState, sizeof(BYTE) * 256);
	ZeroMemory(m_byCheckOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsLeave, sizeof(BYTE) * 260);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}

CInput::~CInput(void)
{
}


BYTE CInput::Get_DIKeyState(unsigned char byKeyID)
{
	return m_byKeyState[byKeyID];
}

BYTE CInput::Get_DIMouseState(MOUSEKEYSTATE byMouseID)
{
	return m_MouseState.rgbButtons[byMouseID];
}

LONG CInput::Get_DIMouseMove(MOUSEMOVESTATE byMouseMoveState)
{
	return *(((LONG*)(&m_MouseState)) + byMouseMoveState);
}

HRESULT CInput::Ready_InputDevice(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, NULL)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, NULL)))
		return E_FAIL;

	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoard->Acquire();

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, NULL)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

void CInput::SetInputState(void)
{
	ResetOnce();
	SetAcquire();

	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);

	SetInputOnce();
}

void CInput::Reset_InputState(void)
{
	ZeroMemory(m_byKeyState, sizeof(BYTE) * 256);
	ZeroMemory(m_byCheckOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsOnce, sizeof(BYTE) * 260);
	ZeroMemory(m_byIsLeave, sizeof(BYTE) * 260);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}

void CInput::Set_Acquire(void)
{
	m_pKeyBoard->Acquire();
	m_pMouse->Acquire();
}

void CInput::ResetOnce(void)
{
	for (int b = 0; b < 256; ++b)
	{
		m_byIsOnce[b] = false;
		m_byIsLeave[b] = false;
		if (!m_byKeyState[b])
		{
			if (m_byCheckOnce[b])
				m_byIsLeave[b] = true;
			m_byCheckOnce[b] = false;
		}
	}

	for (int b = 0; b < 4; ++b)
	{
		m_byIsOnce[b + 256] = false;
		m_byIsLeave[b + 256] = false;
		if (!m_MouseState.rgbButtons[b])
		{
			if (m_byCheckOnce[b + 256])
				m_byIsLeave[b + 256] = true;
			m_byCheckOnce[b + 256] = false;
		}
	}
}

void CInput::SetAcquire(void)
{
	if (m_pKeyBoard)
		m_pKeyBoard->Acquire();

	if (m_pMouse)
		m_pMouse->Acquire();
}

void CInput::SetInputOnce(void)
{
	for (int b = 0; b < 256; ++b)
	{
		if (m_byKeyState[b] && !m_byCheckOnce[b])
		{
			m_byIsOnce[b] = true;
			m_byCheckOnce[b] = true;
		}
		else if (m_byKeyState[b] && m_byCheckOnce[b])
			m_byIsOnce[b] = false;
	}

	for (int b = 0; b < 4; ++b)
	{
		if (m_MouseState.rgbButtons[b] && !m_byCheckOnce[b + 256])
		{
			m_byIsOnce[b + 256] = true;
			m_byCheckOnce[b + 256] = true;
		}
		else if (m_MouseState.rgbButtons[b] && m_byCheckOnce[b + 256])
			m_byIsOnce[b + 256] = false;
	}
}

BYTE CInput::GetDIKeyStateOnce(BYTE KeyFlag)
{
	return m_byIsOnce[KeyFlag];
}

BYTE CInput::GetDIKeyStateLeave(BYTE KeyFlag)
{
	return m_byIsLeave[KeyFlag];
}

BYTE CInput::GetDIMouseStateOnce(BYTE KeyFlag)
{
	return m_byIsOnce[KeyFlag + 256];
}

BYTE CInput::GetDIMouseStateLeave(BYTE KeyFlag)
{
	return m_byIsLeave[KeyFlag + 256];
}

void CInput::Release(void)
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pInputSDK);

	delete this;
}