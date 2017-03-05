/* RenderThread class */

#include "CPreRender.h"
#include "engine\CPreRenderThread.h"

CPreRender::CPreRender()
{
	CPreRenderThread::Instance()->AddRenderThread( this );
}

CPreRender::~CPreRender()
{
}
