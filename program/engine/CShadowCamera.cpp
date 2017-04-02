/* ShadowCamera class */

#include "CShadowCamera.h"
#include "CCamera.h"
#include "CWindow.h"
#include "CDevice.h"
#include "CMouse.h"

namespace
{
	const DirectX::XMFLOAT4 g_f4DefPos	= DirectX::XMFLOAT4( 2.0f, 2.0f, 2.0f, 0.0f );
	const float g_fDefRot		= 0.0f;
	const float g_fDefZoom		= 1.0f;
}

CShadowCamera::CShadowCamera()
	: m_uWidth				( 0 )
	, m_uHeight				( 0 )
	, m_bRotation			( false )
{
}

CShadowCamera::~CShadowCamera()
{
}

HRESULT CShadowCamera::Init()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect( CWindow::Instance()->GetHWnd(), &rc );
	m_uWidth	= rc.right - rc.left;
	m_uHeight	= rc.bottom - rc.top;
	m_stShwCam.m_fRot	= g_fDefRot;
	m_stShwCam.m_fZoom	= g_fDefZoom;

	// XV.
	Change();

	return hr;
}

void CShadowCamera::Change()
{
	// Set
	CLight* pcLight = CLight::Instance();
	if( pcLight != nullptr )
	{
		m_stShwCam.m_f4CamPos.x = g_f4DefPos.x * pcLight->m_f4LightVec[CLight::enLight0].x;
		m_stShwCam.m_f4CamPos.y = g_f4DefPos.y * pcLight->m_f4LightVec[CLight::enLight0].y;
		m_stShwCam.m_f4CamPos.z = g_f4DefPos.z * pcLight->m_f4LightVec[CLight::enLight0].z;
	}

	float fZoom = CCamera::Instance()->m_fZoom;

	// Initialize the view matrix
	DirectX::XMVECTOR Eye	= DirectX::XMVectorSet( m_stShwCam.m_f4CamPos.x, m_stShwCam.m_f4CamPos.y, m_stShwCam.m_f4CamPos.z, 0.0f );
	DirectX::XMVECTOR At	= DirectX::XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	DirectX::XMVECTOR Up	= DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	m_stShwCam.m_mView		= DirectX::XMMatrixLookAtLH( Eye, At, Up );

	// Initialize the projection matrix
	m_stShwCam.m_mProjection = DirectX::XMMatrixOrthographicLH(
		static_cast<FLOAT>( m_uWidth ) * 0.02f * fZoom,
		static_cast<FLOAT>( m_uHeight ) * 0.02f * fZoom, 
		-20.0f, 
		20.0f );
}
