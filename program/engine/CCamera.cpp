/* Camera class */

#include "CWindow.h"
#include "CCamera.h"
#include "CDevice.h"
#include "CMouse.h"

namespace
{
	const XMFLOAT4 g_f4DefPos	= XMFLOAT4( 5.0f, 3.0f, 5.0f, 0.0f );
	const float g_fDefRot		= 0.0f;
	const float g_fDefZoom		= 1.0f;
}

CCamera::CCamera()
	: m_pCbViewProjection	( nullptr )
	, m_fRot				( 0.0f )
	, m_fZoom				( 0.0f )
	, m_uWidth				( 0 )
	, m_uHeight				( 0 )
	, m_bRotation			( false )
{
	m_mView			= XMMatrixIdentity();
	m_mProjection	= XMMatrixIdentity();
	m_f4CalPos		= XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4CamPos		= XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4Lookat		= XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );

	// Set
	m_f4CalPos.x	= g_f4DefPos.x;
	m_f4CalPos.y	= g_f4DefPos.y;
	m_f4CalPos.z	= g_f4DefPos.z;
}

CCamera::~CCamera()
{
	I_RELEASE( m_pCbViewProjection );
}

void CCamera::Update()
{
	if( CMouse::Instance()->GetState() == CMouse::enState_ClickeLeft )
	{
		m_fRot += (float)CMouse::Instance()->m_iMoveX * 0.005f;
		m_f4CalPos.y += (float)CMouse::Instance()->m_iMoveY * 0.01f;
	}
	if( CMouse::Instance()->GetState() == CMouse::enState_ClickeCenter )
	{
		m_f4Lookat.y += (float)CMouse::Instance()->m_iMoveY * 0.01f;
		m_f4CalPos.y += (float)CMouse::Instance()->m_iMoveY * 0.01f / m_fZoom;
	}
	if( CMouse::Instance()->GetState() == CMouse::enState_ClickeRight )
	{
		m_bRotation ^= 0x1;
	}
	if( m_bRotation )
	{
		m_fRot += 0.003f;
	}
	m_fZoom -= (float)CMouse::Instance()->m_iRot * 0.0005f;
	m_fZoom = ( m_fZoom < 0.1f )?		0.1f		: m_fZoom;
	m_fRot = ( m_fRot > 2.0f * I_PI )?	0.0f		: m_fRot;
	m_fRot = ( m_fRot < 0.0f )?			2.0f * I_PI	: m_fRot;
}

HRESULT CCamera::Init( ID3D11DeviceContext* _pContext )
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags	= 0;

	bd.ByteWidth		= sizeof( StViewProjection );
	I_RETURN( pcDevice->m_pd3dDevice->CreateBuffer( &bd, NULL, &m_pCbViewProjection ) );

	RECT rc;
	GetClientRect( CWindow::Instance()->GetHWnd(), &rc );
	m_uWidth	= rc.right - rc.left;
	m_uHeight	= rc.bottom - rc.top;

	m_fRot	= g_fDefRot;
	m_fZoom	= g_fDefZoom;

	// Update
	Update( _pContext );

	return hr;
}

void CCamera::Update( ID3D11DeviceContext* _pContext )
{
	// Initialize the view matrix
	m_f4CamPos.x	= sin( m_fRot ) * m_f4CalPos.x * m_fZoom;
	m_f4CamPos.y	= m_f4CalPos.y * m_fZoom;
	m_f4CamPos.z	= cos( m_fRot ) * m_f4CalPos.z * m_fZoom;
	XMVECTOR Eye	= XMVectorSet( m_f4CamPos.x, m_f4CamPos.y, m_f4CamPos.z, 0.0f );
	XMVECTOR At		= XMVectorSet( m_f4Lookat.x, m_f4Lookat.y, m_f4Lookat.z, 0.0f );
	XMVECTOR Up		= XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	m_mView			= XMMatrixLookAtLH( Eye, At, Up );
	
	// ÉrÉÖÅ[èÓïÒ
	m_stViewProjections.m_f4TexSize.x	= WIDTH;
	m_stViewProjections.m_f4TexSize.y	= HEIGHT;
	m_stViewProjections.m_mView			= XMMatrixTranspose( m_mView );

	// Initialize the projection matrix
	m_mProjection = XMMatrixPerspectiveFovLH( 
		XM_PIDIV4, 
		static_cast<FLOAT>( m_uWidth ) / static_cast<FLOAT>( m_uHeight ), 
		0.01f, 
		100.0f );	
	m_stViewProjections.m_mProjection = XMMatrixTranspose( m_mProjection );

	// Camera Pos
	m_stViewProjections.m_fCameraPos = XMFLOAT4( m_f4CamPos.x, m_f4CamPos.y, m_f4CamPos.z, m_f4CamPos.w );

	// Update
	_pContext->UpdateSubresource( 
		m_pCbViewProjection, 
		0, NULL, 
		&m_stViewProjections, 
		0, 0 );

	_pContext->VSSetConstantBuffers( 0, 1, &m_pCbViewProjection );
	_pContext->PSSetConstantBuffers( 0, 1, &m_pCbViewProjection );
}
