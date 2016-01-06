#include "MyDirectX.h"
#include "MyInput.h"
using namespace std;

bool gameover = false;

//Windows event handling function
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
		break;
	case WM_INPUT:
		input1.GetData(lParam);
		break;
	};
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Main Windows entry function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;

	MSG msg;

	//set the window's properties
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "MainWindowClass";
	RegisterClassEx(&wc);

	//create a new window
	//previously found in InitInstance function

	HWND window = CreateWindow("MainWindowClass",
		APPTITLE.c_str(),
		WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT,
		SCREENW,
		SCREENH,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (window == 0) return 0;

	//display the window

	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	//initialize the game
	if (!Game_Init(window)) return 0;

	//main message loop
	while (!gameover)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Game_Run(window);
	}
	Game_End();

	return msg.wParam;

}