/* main loop class */

#include <windows.h>
#include "CMainLoop.h"
#include "object\CRender.h"
#include "object\CUpdate.h"
#include "object\CSequence.h"
#include "sequence\CGraphicsDemo.h"
#include "engine\CPreRenderThread.h"
#include "engine\CDevice.h"
#include "engine\CCamera.h"
#include "engine\CWindow.h"

CMainLoop::CMainLoop()
{
}

CMainLoop::~CMainLoop()
{
	Release();
}

void CMainLoop::Begine()
{
	SetSequence( I_NEW( CGraphicsDemo ) );
}

void CMainLoop::Release()
{
	listUpdate.clear();
	listRender.clear();
	I_DELETE( m_pcSequence );
}

int CMainLoop::MainLoop()
{
	CDevice* pcDevice = CDevice::Instance();
	CPreRenderThread* pcPreRenderThrea = CPreRenderThread::Instance();
	CCamera::Instance()->Init( pcDevice->m_pImmediateContext );

	MSG msg = { 0 };
	while( WM_QUIT != msg.message )
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else if( CWindow::Instance()->GetActive() )
		{
			// Update
			for( auto obj = listUpdate.begin(); obj != listUpdate.end(); obj++ )
			{
				if( *obj != nullptr )
				{
					(*obj)->Update();
				}
			}

			if( pcDevice->m_pImmediateContext != nullptr )
			{
				// Rnder Clear
				float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
				pcDevice->m_pImmediateContext->ClearState();
				pcDevice->m_pImmediateContext->ClearRenderTargetView(
					pcDevice->m_pRenderTargetView, ClearColor );
			
				// Multi Thread Pre Render
				for( int ii = 0; ii < CPreRenderThread::enMode_Max; ii++ )
				{
					SetEvent( pcPreRenderThrea->m_hBeginEvent[ii] );
				}
				WaitForMultipleObjects(
					pcPreRenderThrea->m_iCnt, pcPreRenderThrea->m_hEndEvent, 
					TRUE, INFINITE );
				for( int ii = 0; ii < pcPreRenderThrea->m_iCnt; ii++ )
				{
					if( pcPreRenderThrea->m_pCommandList[ii] == nullptr )
					{
						break;
					}
					pcDevice->m_pImmediateContext->ExecuteCommandList(
						pcPreRenderThrea->m_pCommandList[ii], false );
					I_RELEASE( pcPreRenderThrea->m_pCommandList[ii] );
					ResetEvent( pcPreRenderThrea->m_hEndEvent[ii] );
				}
			
				// Render
				for( auto obj = listRender.begin(); obj != listRender.end(); obj++ )
				{
					if( *obj != nullptr )
					{
						(*obj)->Render( pcDevice->m_pImmediateContext );
					}
				}
			
				// Swap
				pcDevice->m_pSwapChain->Present( 0, 0 );
			}
		}
	}
	return static_cast<int>( msg.wParam );
}

void CMainLoop::SetSequence( CSequence* _obj )
{
	m_pcSequence = _obj;
}

void CMainLoop::AddUpdate( CUpdate* _obj )
{
	listUpdate.push_back( _obj );
}

void CMainLoop::AddRender( CRender* _obj )
{
	listRender.push_back( _obj );
}

