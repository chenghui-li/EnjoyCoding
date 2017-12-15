#include "MyDirectX.h"
#include "emy.h"
using namespace std;
bool gameover = false;

//Window callback function
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
void getemy(int x, int y, int blood, int kill) {


}
//Main Windows entry function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE nPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG msg;
	//set the new window's properties
	//previously found in the MyregisterClass function
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "MainWindowClass";
	//	getemy(0, 0, Ablood->blood,Abullet->kill);
	if (!RegisterClassEx(&wc))
		return FALSE;
	//previously found in the InitInstance function
	HWND window = CreateWindow("MainWindowClass", APPTITLE.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, SCREENW, SCREENH, (HWND)NULL, (HMENU)NULL, hInstance, (LPVOID)NULL);
	if (window == 0)return 0;
	//display the window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);
	//intialize the game
	if (!Game_Init(window)) return 0;

	//main game loop
	while (!gameover)
	{
		//process Window events
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//process game loop
		Game_Run(window);
	}
	//shut down
	Game_End();
	//free game resources
	return msg.wParam;
}



