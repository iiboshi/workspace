/* Light class */

#include "CLight.h"

namespace
{
	#if defined( USE_SHADOW )
	const float g_fMaxPow	= 2.5f;
	const float g_fMaxSpec	= 0.8f;
	#elif 0
	const float g_fMaxPow	= 15.0f;
	const float g_fMaxSpec	= 0.8f;
	const float g_fMaxAO	= 0.15f;
	#else
	const float g_fMaxPow	= 3.0f;
	const float g_fMaxSpec	= 0.8f;
	const float g_fMaxAO	= 0.15f;
	#endif

	const DirectX::XMFLOAT4 g_f4MaxAO = DirectX::XMFLOAT4( 0.4f, 0.15f, 0.13f, 0.15f );	// xyz:Color w:Pow
}

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
	m_f4LightCol[enLight0]	= DirectX::XMFLOAT4( g_fMaxPow, g_fMaxPow, g_fMaxPow, g_fMaxSpec );
	m_f4LightCol[enLight1]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4AO					= g_f4MaxAO;
	#elif 0
	float fPow				= g_fMaxPow / ( enLightNum + 1 ) / 3.0f;
	float fSpec				= g_fMaxSpec / ( enLightNum + 1 );
	m_f4MainCol				= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[enLight0]	= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[enLight1]	= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4AO					= g_f4MaxAO;
	#else
	m_f4MainCol				= DirectX::XMFLOAT4( g_fMaxPow, g_fMaxPow, g_fMaxPow, g_fMaxSpec );
	m_f4LightCol[enLight0]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4LightCol[enLight1]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4AO					= g_f4MaxAO;
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
	m_fRot += 0.01f;
	if( m_fRot > 2.0f * I_PI ) {
		m_fRot = 0.0f;
	}
	#endif
}
