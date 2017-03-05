/* window class */

#include "CWindow.h"
#include "CMouse.h"

bool CWindow::m_bActive;

CWindow::CWindow()
	: m_hInst	( NULL )
	, m_hWnd	( NULL )
{
};

CWindow::~CWindow()
{
	DestroyWindow( m_hWnd );
};

HRESULT CWindow::InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof( WNDCLASSEX );
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CWindow::WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon( hInstance, IDI_APPLICATION );
	wcex.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground	= ( HBRUSH )( COLOR_WINDOW + 1 );
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WINDOWCLASSNAME;
	wcex.hIconSm		= LoadIcon( wcex.hInstance, IDI_APPLICATION );
	if( !RegisterClassEx( &wcex ) )
	{
		return E_FAIL;
	}

	// Create window
	m_hInst = hInstance;
	RECT rc = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	m_hWnd = CreateWindow(
		WINDOWCLASSNAME,
		WINDOWNAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		NULL, NULL,
		hInstance,
		NULL );

	if( !m_hWnd )
	{
		return E_FAIL;
	}

	ShowWindow( m_hWnd, nCmdShow );

	return S_OK;
}

LRESULT CALLBACK CWindow::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	CMouse::Instance()->Update( hWnd, message, wParam, lParam );

	switch( message ) {

	case WM_PAINT: {
			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
		} break;

	case WM_DESTROY: {
			PostQuitMessage( 0 );
		} break;

	case WM_SIZE: {
			switch( wParam ) {
			case SIZE_MINIMIZED: { m_bActive = false; } break;
			default: break;
			}
		} break;

	case WM_ACTIVATE: {
			switch( wParam ) {
			case WA_INACTIVE: { m_bActive = false; } break;
			case WA_ACTIVE:
			case WA_CLICKACTIVE: { m_bActive = true; } break;
			default: break;
			}
		} break;

	case WM_ENTERSIZEMOVE: {
			m_bActive = false;
		} break;

	case WM_EXITSIZEMOVE: {
			m_bActive = true;
		} break;

	default: { return DefWindowProc( hWnd, message, wParam, lParam ); }
	}

	return 0;
}
