#include "MyInput.h"

cInputDevices::cInputDevices()
	: m_nMouseXChange(0), m_nMouseYChange(0), m_nMouseX(0), m_nMouseY(0), m_bLMB(0), m_bRMB(0)
{
	// Keyboard
	m_Rid[0].usUsagePage = 1;
	m_Rid[0].usUsage = 6;
	m_Rid[0].dwFlags = 0;
	m_Rid[0].hwndTarget = NULL;

	// Mouse
	m_Rid[1].usUsagePage = 1;
	m_Rid[1].usUsage = 2;
	m_Rid[1].dwFlags = 0;
	m_Rid[1].hwndTarget = NULL;

	ResetAllKeyData();
}

cInputDevices::~cInputDevices()
{
}

void cInputDevices::ResetAllKeyData()
{
	// Set all the following to false to be safe

	for (int iii = 0; iii < 25; ++iii)
		m_baAlphabet[iii] = 0;

	for (int iii = 0; iii < 4; ++iii)
		m_baArrows[iii] = 0;

}

INT cInputDevices::RegisterDevices()
{
	if (RegisterRawInputDevices(m_Rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
		return 1;

	return 0;
}

void cInputDevices::GetData(LPARAM lParam)
{
	// Determine how big the buffer should be
	UINT bufferSize;
	GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));

	// Create a buffer of the correct size - but see note below

	// Call the function again, this time with the buffer to get the data
	if (bufferSize <= 40)
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)m_buffer, &bufferSize, sizeof(RAWINPUTHEADER));

	RAWINPUT *raw = (RAWINPUT*)m_buffer;

	// The mouse has not been tested extensively,
	// but I believe it works.
	if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		// Get values from the mouse member (of type RAWMOUSE)
		m_nMouseXChange = raw->data.mouse.lLastX;
		m_nMouseYChange = raw->data.mouse.lLastY;



		bool bStateDown = raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN;
		bool bStateUp = raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP;

		if (bStateDown == true && bStateUp == false)
		{
			m_bLMB = true;
		}

		if (bStateUp == true)
		{
			m_bLMB = false;
		}


		bool bStateDownTwo = raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN;
		bool bStateUpTwo = raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP;

		if (bStateDownTwo == true && bStateUpTwo == false)
		{
			m_bRMB = true;
		}

		if (bStateUpTwo == true)
		{
			m_bRMB = false;
		}


	}

	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		// Get key value from the keyboard member (of type RAWKEYBOARD)
		USHORT keyCode = raw->data.keyboard.VKey;
		bool keyUp = raw->data.keyboard.Flags & RI_KEY_BREAK;

		// Ok because I put the most effort into this section of my code,
		// I will briefly explain.

		// We need a boolean pointer to start.
		bool* pbToKey = NULL;

		// The keycode can be one of many possibilities,
		// which is why a switch case is to exhaustive to
		// use and I believe for the most part switch case 
		// would not be efficient.

		// So instead of manually typing every possible
		// case value, we can just start by looping through
		// an expected range of keys the keycode might be.

		for (int iii = 0; iii < 25; ++iii)
		{
			// We add the hex-code 0x41 because that is the
			// value MSDN lists for virtual keycode 'A'.
			if (keyCode == iii + 0x41)
			{
				// However our alphabet or array of booleans that
				// represent it, begins at 0 and is only 25 in length.
				// So iii itself is the appropritate index.
				pbToKey = &m_baAlphabet[iii];

				// At this point we have assigned our boolean pointer variable
				// a new address which is whatever index iii would be accessing.
				// We break because there is no need to go further since we found the
				// matching keycode!
				break;
			}
		}

		// However, if the whole for loop above did not find any matches
		// to the keycode, then we can expect our boolean pointer to be the same
		// value we initalized it to. Which would be NULL.

		if (pbToKey != NULL)
		{
			// if we entered this block of code, then the boolean pointer
			// is not NULL and has been assigned. The main idea is to
			// call the CheckKeyPress function to update the boolean pointed to
			// by the pbToKey, as well as pass in that boolean value itself.
			*pbToKey = CheckKeyPress(*pbToKey, keyUp);

			// Be sure to return ASAP!
			return;
		}

		// If the boolean pointer was null, then before giving up
		// we can check if the keycode falls in the arrow key range!

		for (int iii = 0; iii < 4; ++iii)
		{
			// Notice we add 0x25 to iii,
			// just like we added 0x41 to iii in the last for loop.

			if (keyCode == iii + 0x25)
			{
				// Yay found it!
				pbToKey = &m_baArrows[iii];
				break;
			}
		}

		// Because the keycode may not match any in the ranges we
		// so far, we check to see if the boolean
		// pointer has been assigned anything other than NULL. If 
		// it is NULL then we do not want to call CheckKeyPress.
		// The reason for that is, it would error because our pointer
		// is invalid and CheckKeyPress would be fail.
		if (pbToKey != NULL)
		{

			*pbToKey = CheckKeyPress(*pbToKey, keyUp);

			// Be sure to return ASAP!
			return;
		}

		// TODO: Add more key ranges to check for! Just follow
		// how I have set things up so far.

	}


}

bool cInputDevices::CheckKeyPress(bool bLastKeyState, bool bThisKeyState)
{
	// The following may be overkill, but just know the value returned
	// is based upon the current state of the boolean in question, and the
	// new state we have processed.
	if (bThisKeyState == false)
	{
		if (bLastKeyState == true)
		{
			return true;
		}
		else
		{
			return true;
		}
	}
	else if (bThisKeyState == true)
	{
		if (bLastKeyState == false)
		{
			return false;
		}
		else
			return false;
	}
}