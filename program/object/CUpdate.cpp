/* update class */

#include "CUpdate.h"
#include "main\CMainLoop.h"

CUpdate::CUpdate()
{
	CMainLoop::Instance()->AddUpdate( this );
}

CUpdate::~CUpdate()
{
}
