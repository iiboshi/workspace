/* Light class */

#include "CLight.h"

CLight::CLight()
	: m_fRot ( 0.0f )
{
	// Light
	m_f4MainVec				= DirectX::XMFLOAT4( 1.0f,	1.0f,	1.0f,	1.0f );
	m_f4LightVec[enLight0]	= DirectX::XMFLOAT4( 1.0f,	1.0f,	1.0f,	1.0f );
	m_f4LightVec[enLight1]	= DirectX::XMFLOAT4( -1.0f,	1.0f,	-1.0f,	1.0f );

	// Color
	#if defined( USE_SHADOW )
	m_f4MainCol				= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4LightCol[enLight0]	= DirectX::XMFLOAT4( 3.5f, 3.5f, 3.5f, 1.0f );
	m_f4LightCol[enLight1]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4AO					= DirectX::XMFLOAT4( 0.4f, 0.15f, 0.13f, 0.15f );	// xyz:Color w:Pow;
	#elif 0
	float fPow				= 15.0f / ( enLightNum + 1 ) / 3.0f;
	float fSpec				= 0.8f / ( enLightNum + 1 );
	m_f4MainCol				= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[enLight0]	= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[enLight1]	= DirectX::XMFLOAT4( 0.4f, 0.15f, 0.13f, 0.2f );
	m_f4AO					= g_f4MaxAO;
	#else
	m_f4MainCol				= DirectX::XMFLOAT4( 2.0f, 2.0f, 2.0f, 0.8f );
	m_f4LightCol[enLight0]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4LightCol[enLight1]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4AO					= DirectX::XMFLOAT4( 0.4f, 0.15f, 0.13f, 0.2f );
	#endif
}

CLight::~CLight()
{
}

void CLight::Update()
{
	#if 1
	m_f4LightVec[enLight0].x = cosf( m_fRot ) * 1.0f;
	m_f4LightVec[enLight0].z = sinf( m_fRot ) * 1.0f;
	m_fRot += 0.001f;
	if( m_fRot > 2.0f * I_PI ) {
		m_fRot = 0.0f;
	}
	#endif
}
