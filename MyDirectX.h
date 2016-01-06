#pragma once

//header files 
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <xinput.h>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <mmsystem.h>
#include "MyInput.h"
#include <array>
#include <algorithm>

using namespace std;

//libraries
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")


extern const string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
extern bool gameover;

//Direct3D objects
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 backbuffer;
extern LPD3DXSPRITE spriteobj;

extern cInputDevices input1;

//Direct3D functions
bool Direct3D_Init(HWND hwnd, int width, int height, bool fullscreen);
void Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(string filename);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);
D3DXVECTOR2 GetBitmapSize(string filename);
LPDIRECT3DTEXTURE9 LoadTexture(string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));

extern int JumpTime;
extern float Grav_velc;
void Gravity(int &starttime);

enum PLAYER_STATES
{
	STANDING = 0,
	RUNNING,
	JUMPING,
	DASHING,
	HEAVY_ATK,
	LIGHT_ATK,
	NOTHING
};
//sprite structure
struct SPRITE
{
	float x, y;
	double Pos_X, Pos_Y;
	int frame, columns;
	int width, height, lBound, rBound, tBound, bBound;
	float scalingX, scalingY, rotation;
	int startframe, endframe;
	int starttime, GravTime, delay;
	int direction;
	float velx, vely;
	D3DCOLOR color;

	SPRITE()
	{
		frame = 0;
		columns = 1;
		width = height = 0;
		scalingX = 1.0f;
		scalingY = 1.0f;
		rotation = 0.0f;
		startframe = endframe = 0;
		direction = 1;
		starttime = GravTime = delay = 0;
		velx = vely = 0.0f;
		color = D3DCOLOR_XRGB(255, 255, 255);
		lBound = rBound = tBound = bBound = 0;
	}
};
//Sprite functions
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns);
void Sprite_Animate(int&frame, int startframe, int endframe, int direction, int &starttime, int delay);
void Sprite_Animate_Once(int &frame, int startframe, int endframe, int direction, int &starttime, int delay);
void Sprite_Transform_Draw(
	LPDIRECT3DTEXTURE9 image,
	int x,
	int y,
	int width,
	int height,
	int frame = 0,
	int columns = 1,
	float rotation = 0.0f,
	float scalingX = 1.0f,
	float scalingY = 1.0f,
	D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255)
	);
//bound box collision
int Collision(SPRITE sprite1, SPRITE sprite2);
bool CollisionR(SPRITE sprite1, SPRITE sprite2);

//font functions
LPD3DXFONT MakeFont(string name, int size);
void FontPrint(
	LPD3DXFONT font,
	int x,
	int y,
	string text,
	D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255)
	);

//DirectInput objects, devices, and states
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;
extern XINPUT_GAMEPAD controllers[4];

//DirectInput functions
bool DirectInput_Init(HWND);
void DirectInput_Update();
void DirectInput_Shutdown();
int Key_Down(int);
int Key_Down_Once(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();
void XInput_Vibrate(int contNum = 0, int amount = 65535);
bool XInput_Controller_Found();

//game functions
bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();
