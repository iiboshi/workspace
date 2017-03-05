#include <windows.h>
#include "engine\CWindow.h"
#include "engine\CDevice.h"
#include "engine\CPreRenderThread.h"
#include "engine\CShader.h"
#include "CMainLoop.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	int iRet;
	HRESULT hr;
	I_RETURN_C( CWindow::Instance()->InitWindow( hInstance, nCmdShow ), 0 );
	I_RETURN_FNC_C( CDevice::Instance()->InitDevice(), CDevice::Instance()->CleanupDevice(), 0 );
	CMainLoop::Instance()->Begine();
	I_RETURN_C( CPreRenderThread::Instance()->InitRenderThreads(), 0 );
	iRet = CMainLoop::Instance()->MainLoop();
	CPreRenderThread::Instance()->DestroyRenderThreads();
	CDevice::Instance()->CleanupDevice();
	return iRet;
}
