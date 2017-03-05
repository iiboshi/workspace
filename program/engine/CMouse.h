/* Mouse class */

#ifndef H_CMOUSE
#define H_CMOUSE

#include "object\CUpdate.h"
#include <windows.h>

class CMouse : public CUpdate
{
I_SINGLETON( CMouse )
public:
	enum EnState
	{
		enState_None,
		enState_ClickeLeft,
		enState_ClickeRight,
		enState_ClickeCenter,
		enState_Wheel,
	};
	CMouse();
	~CMouse();
	void Update();
	void Update( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	void SetState( EnState _eState ){ m_eState = _eState; }
	EnState GetState(){ return m_eState; }
public:
	EnState	m_eState;
	int		m_iPosX;
	int		m_iPosY;
	int		m_iMoveX;
	int		m_iMoveY;
	int		m_iRot;
	int		m_iDelta;
	bool	m_bMove;
	bool	m_bRot;
};

#endif
