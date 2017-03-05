/* Mouse class */

#include "CMouse.h"
#include "CWindow.h"
#include <WindowsX.h>

CMouse::CMouse()
	: m_eState	( enState_None )
	, m_iPosX	( 0 )
	, m_iPosY	( 0 )
	, m_iMoveX	( 0 )
	, m_iMoveY	( 0 )
	, m_iRot	( 0 )
	, m_iDelta	( 0 )
	, m_bMove	( false )
	, m_bRot	( false )
{
}

CMouse::~CMouse()
{
}

void CMouse::Update()
{
	if( m_bMove )
	{
		m_bMove = false;
	}
	else
	{
		m_iMoveX = 0;
		m_iMoveY = 0;
	}

	if( m_bRot )
	{
		m_bRot = false;
	}
	else
	{
		m_iRot = 0;
	}
}

void CMouse::Update( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_LBUTTONDOWN: { m_eState = enState_ClickeLeft;	} break;
	case WM_RBUTTONDOWN: { m_eState = enState_ClickeRight;	} break;
	case WM_MBUTTONDOWN: { m_eState = enState_ClickeCenter;	} break;
	case WM_LBUTTONUP:	 { m_eState = enState_None;			} break;
	case WM_RBUTTONUP:	 { m_eState = enState_None;			} break;
	case WM_MBUTTONUP:	 { m_eState = enState_None;			} break;
	case WM_MOUSELEAVE:	 { m_eState = enState_None;			} break;
	case WM_NCMOUSEMOVE: { m_eState = enState_None;			} break;
	case WM_MOUSEMOVE:
		{
			m_bMove = true;
			m_iMoveX = GET_X_LPARAM(lParam) - m_iPosX;
			m_iMoveY = GET_Y_LPARAM(lParam) - m_iPosY;
			m_iPosX = GET_X_LPARAM(lParam);
			m_iPosY = GET_Y_LPARAM(lParam);
		} break;
	case WM_MOUSEWHEEL:
		{
			m_bRot		= true;
			m_iRot		= GET_WHEEL_DELTA_WPARAM(wParam) - m_iRot;
			m_iDelta	= GET_WHEEL_DELTA_WPARAM(wParam);
		} break;
	default:
		break;
	}
}
