/* RenderThread class */

#include <process.h>
#include "CPreRenderThread.h"
#include "CDevice.h"

bool						CPreRenderThread::m_bLoop[CPreRenderThread::enMode_Max];
ID3D11DeviceContext*		CPreRenderThread::m_pDeviceContext[CPreRenderThread::enMode_Max];
ID3D11CommandList*			CPreRenderThread::m_pCommandList[CPreRenderThread::enMode_Max];
int							CPreRenderThread::m_iThreadData[CPreRenderThread::enMode_Max];
HANDLE						CPreRenderThread::m_hThread[CPreRenderThread::enMode_Max];
HANDLE						CPreRenderThread::m_hBeginEvent[CPreRenderThread::enMode_Max];
HANDLE						CPreRenderThread::m_hEndEvent[CPreRenderThread::enMode_Max];
std::vector<CPreRender*>	CPreRenderThread::m_listRendrThread;

CPreRenderThread::CPreRenderThread()
	: m_iCnt	( 0 )
{
	for( int ii = 0; ii < enMode_Max; ii++ )
	{
		m_bLoop[ii]				= true;
		m_pCommandList[ii]		= nullptr;
		m_pDeviceContext[ii]	= nullptr;
		m_iThreadData[ii]		= 0;
	}
}

CPreRenderThread::~CPreRenderThread()
{
	for( int ii = 0; ii < enMode_Max; ii++ )
	{
		I_RELEASE( m_pCommandList[ii] );
		I_RELEASE( m_pDeviceContext[ii] );
	}
}

void CPreRenderThread::DestroyRenderThreads()
{
	for( int ii = 0; ii < enMode_Max; ii++ )
	{
		CloseHandle( m_hBeginEvent[ii] );
		CloseHandle( m_hEndEvent[ii] );
		CloseHandle( m_hThread[ii] );
	}
}

HRESULT CPreRenderThread::InitRenderThreads()
{
	for( auto obj = m_listRendrThread.begin(); obj != m_listRendrThread.end(); obj++ )
	{
		m_iCnt++;
	}
	if( m_iCnt == 0 || m_iCnt >= enMode_Max )
	{
		return S_FALSE;
	}
	HRESULT hr;
	ID3D11Device* pd3dDevice = CDevice::Instance()->m_pd3dDevice;
	I_NULLCHECK( pd3dDevice, S_FALSE )
	for ( int ii = 0; ii < m_iCnt; ii++ )
	{
		m_iThreadData[ii] = ii;
		m_hBeginEvent[ii] = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_hEndEvent[ii] = CreateEvent( NULL, TRUE, FALSE, NULL );
		I_RETURN( pd3dDevice->CreateDeferredContext( 0, &m_pDeviceContext[ii] ) );
		m_hThread[ii] = ( HANDLE )_beginthreadex( 
			NULL, 
			0, 
			CPreRenderThread::RenderFunction, 
			&m_iThreadData[ii], 
			CREATE_SUSPENDED, 
			NULL );
		ResumeThread( m_hThread[ii] );
	}
	return S_OK;
}

unsigned int WINAPI CPreRenderThread::RenderFunction( LPVOID lpParameter )
{
	const EnMode enMode = *( static_cast<EnMode*>( lpParameter ) );
	ID3D11DeviceContext* pd3dDeferredContext = m_pDeviceContext[enMode];
	ID3D11CommandList*& pd3dCommandList = m_pCommandList[enMode];
	auto obj = m_listRendrThread[enMode];
	while( m_bLoop[enMode] )
	{
		WaitForSingleObject( m_hBeginEvent[enMode], INFINITE );
		pd3dDeferredContext->ClearState();
		obj->Render( pd3dDeferredContext );
		pd3dDeferredContext->FinishCommandList( false, &pd3dCommandList );
		ResetEvent( m_hBeginEvent[enMode] );
		SetEvent( m_hEndEvent[enMode] );
	}
	_endthreadex( 0 );
	return 0;
}

void CPreRenderThread::AddRenderThread( CPreRender* _obj )
{
	m_listRendrThread.push_back( _obj );
	
	// è„å¿ämîF.
	if( m_listRendrThread.capacity() == enMode_Max )
	{
		_ASSERT( 0 );
	}
}
