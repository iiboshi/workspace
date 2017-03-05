/* window class */

#ifndef H_CWINDOW
#define H_CWINDOW

#include <windows.h>

class CWindow
{
I_SINGLETON( CWindow )
public:
	CWindow();
	~CWindow();
	HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
	static LRESULT CALLBACK	WndProc( HWND, UINT, WPARAM, LPARAM );
	HWND GetHWnd(){ return m_hWnd; };
	bool GetActive(){ return m_bActive; }
private:
	HINSTANCE	m_hInst;
	HWND		m_hWnd;
	static bool	m_bActive;
};

#endif
