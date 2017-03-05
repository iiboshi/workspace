/* render class */

#include "CRender.h"
#include "main\CMainLoop.h"

CRender::CRender()
{
	CMainLoop::Instance()->AddRender( this );
}

CRender::~CRender()
{
}

