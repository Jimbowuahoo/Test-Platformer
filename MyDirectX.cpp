#include "MyDirectX.h"
#include <iostream>
using namespace std;


//direct3D variables
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPD3DXSPRITE spriteobj = NULL;


//directinput variables
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];
XINPUT_GAMEPAD controllers[4];
PLAYER_STATES pState;

//direct3D initialization
bool Direct3D_Init(HWND window, int width, int height, bool fullscreen)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		MessageBox(window, "Error initializing Direct3D", "Error", MB_OK);
		return 0;
	}

	//set Direct3D presentation parameters

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = (!fullscreen);
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.hDeviceWindow = window;

	//create Direct3D device
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	if (d3ddev == NULL)
	{
		MessageBox(window, "Error creating Direct3D device", "Error", MB_OK);
		return FALSE;
	}
	
	//create pointer to the backbuffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//create sprite object
	D3DXCreateSprite(d3ddev, &spriteobj);

	return true;
}

//Direct3D shutdown
void Direct3D_Shutdown()
{
	if (spriteobj)spriteobj->Release();
	if (d3ddev)d3ddev->Release();
	if (d3d)d3d->Release();
}

//Font functions
LPD3DXFONT MakeFont(string name, int size)
{
	LPD3DXFONT font = NULL;
	D3DXFONT_DESC desc = {
		size,				//height
		0,					//width
		0,					//weight
		0,					//miplevels
		false,				//italic
		DEFAULT_CHARSET,	//Charset
		OUT_TT_PRECIS,		//output precision
		CLIP_DEFAULT_PRECIS,//quality
		DEFAULT_PITCH,		//Pitch and family
		""					//font name
	};
	strcpy(desc.FaceName, name.c_str());
	D3DXCreateFontIndirect(d3ddev, &desc, &font);
	return font;
}
void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color)
{
	//figure out the text boundry
	RECT rect = { x,y,0,0 };
	font->DrawText(NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, color);

	//print the text
	font->DrawText(spriteobj, text.c_str(), text.length(), &rect, DT_LEFT, color);
}

//draw a surface to the screen using StretchRect
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
	//get width and height from source surface
	D3DSURFACE_DESC desc;
	source->GetDesc(&desc);

	//create rects for drawing
	RECT source_rect = { 0,0, (long)desc.Width, (long)desc.Height };
	RECT dest_rect = { (long)x, (long)y, (long)x + desc.Width, (long)y + desc.Height };

	//draw the source surface onto the dest 
	d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);
}

LPDIRECT3DTEXTURE9 LoadTexture(std::string filename, D3DCOLOR transcolor)
{
	LPDIRECT3DTEXTURE9 texture = NULL;

	//get width and height if bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	// create the new texture by loading a bitmap image file
	D3DXCreateTextureFromFileEx(
		d3ddev,					//direct3D device object
		filename.c_str(),		//bitmap filename
		info.Width,				//image Width
		info.Height,			//image height
		1,						//mip-map levels(1 for no chain)
		D3DPOOL_DEFAULT,		//type of surface(standard)
		D3DFMT_UNKNOWN,			//surface format (default)
		D3DPOOL_DEFAULT,		//memory class for the texture
		D3DX_DEFAULT,			//image filter
		D3DX_DEFAULT,			//mip filter
		transcolor,				//color key for transparency
		&info,					//bitmap file info(from loaded file)
		NULL,					//color palette
		&texture);				//destination texture
	if (result != D3D_OK) return NULL;

	return texture;
}

D3DXVECTOR2 GetBitmapSize(string filename)
{
	D3DXIMAGE_INFO info;
	D3DXVECTOR2 size = D3DXVECTOR2(0.0f, 0.0f);
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result == D3D_OK)
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	else
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	return size;
}

//loads a bitmap onto the surface
LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
	LPDIRECT3DSURFACE9 image = NULL;

	//get width and height from the bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	//create surface
	result = d3ddev->CreateOffscreenPlainSurface(
		info.Width,				//width of surface
		info.Height,			//height of surface
		D3DFMT_X8R8G8B8,		//Surface format
		D3DPOOL_DEFAULT,		//memory pool to use
		& image,				//pointer to the surface
		NULL);					//reserved (always NULL)
	
	if (result != D3D_OK) return NULL;

	//load surface form file into newly created surface
	result = D3DXLoadSurfaceFromFile(
		image,					//Destination surface
		NULL,					//Destination palette
		NULL,					//Destination rectangle
		filename.c_str(),		//Source filename
		NULL,					//Source rectangle
		D3DX_DEFAULT,			//controls how the image was filtered
		D3DCOLOR_XRGB(0,0,0),	//for transparency (0 for none)
		NULL);		            //source image info (usually NULL)

	if (result != D3D_OK) return NULL;

	return image;
}

void Sprite_Animate(int &frame, int startframe, int endframe, int direction, int &starttime, int delay)
{
	int TimePassed = 0;
	if ((int)GetTickCount() > starttime + delay)
	{		
		starttime = GetTickCount();
		TimePassed += 1;
		if (frame > endframe || frame < startframe)
			frame = startframe;
		else
		{
			if (frame != endframe+1 && TimePassed < endframe - startframe + 1)
			{
				frame += 1;
			}
			if (frame == endframe + 1)
			{
				TimePassed = 0;
			}
		}
	}
}

void Sprite_Animate_Once(int &frame, int startframe, int endframe, int direction, int &starttime, int delay)
{
	if ((int)GetTickCount() > starttime + delay)
	{
		starttime = GetTickCount();
		if (frame > endframe || frame < startframe)
			frame = startframe;
		else
		{
			if (frame != endframe)
			{
				frame += 1;
			}
		}
	}
}



void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, int frame, int columns, float rotation, float scalingX, float scalingY, D3DCOLOR color)
{
	//create a scale vector
	D3DXVECTOR2 scale(scalingX, scalingY);

	//create a translate vector
	D3DXVECTOR2 trans(x, y);

	//set center by dividing width and height by 2
	D3DXVECTOR2 center((float)(width*scalingX) / 2, (float)(height*scalingY) / 2);
	
	//create 2d transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL,0, &scale, &center, rotation, &trans);

	//tell sprite objects to use the transform
	spriteobj->SetTransform(&mat);

	//calculate frame location in source image
	int fx = (frame%columns)*width;
	int fy = (frame / columns)*height;

	RECT srcRect = { fx,fy,fx + width,fy + height };
		
	//draw the sprite frame
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);
}

//directInput initialization
bool DirectInput_Init(HWND hwnd)
{
	//initialize DirectInput object
	HRESULT result = DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dinput,
		NULL);

	//initialize the keyboard
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();

	//initialize the mouse
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	d3ddev->ShowCursor(false);

	return true;
}

//DirectInput update
void DirectInput_Update()
{
	//update mouse
	dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);

	//update keyboard
	dikeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);

	for (int i = 0; i < 4; i++)
	{
		ZeroMemory(&controllers[i], sizeof(XINPUT_STATE));

		//get the state of the controller
		XINPUT_STATE state;
		DWORD result = XInputGetState(i, &state);

		//store state in global controller array
		if (result == 0) controllers[i] = state.Gamepad;
	}
}

//return mouse x movement
int Mouse_X()
{
	return mouse_state.lX;
}

//return mouse y movement
int Mouse_Y()
{
	return mouse_state.lY;
}

//return mouse button state
int Mouse_Button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}

//return key press state

int Key_Down(int key)
{
	if (keys[key] & 0x80)
	{
		keys[key] = 1;
		return true;
	}
	else
	{
		keys[key] = 0;
		return false;
	}
}

//DirectInput shutdown
void DirectInput_Shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}

	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
}

//returns true if controller id plugged in
bool XInput_Controller_Found()
{
	XINPUT_CAPABILITIES caps;
	ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
	XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps);
	if (caps.Type != 0) return false;
	return true;
}

//Vibrates the controller
void XInput_Vibrate(int contNum, int amount)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = amount;
	vibration.wRightMotorSpeed = amount;
	XInputSetState(contNum, &vibration);
}