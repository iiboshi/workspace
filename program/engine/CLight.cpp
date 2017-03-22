/* Light class */

#include "CLight.h"

namespace
{
	#if defined( USE_SHADOW )
	const float g_fMaxPow	= 3.0f;
	const float g_fMaxSpec	= 0.5f;
	#elif 0
	const float g_fMaxPow	= 15.0f;
	const float g_fMaxSpec	= 2.0f;
	#else
	const float g_fMaxPow	= 3.0f;
	const float g_fMaxSpec	= 1.0f;
	#endif
}

CLight::CLight()
{
	// Light
	m_f4MainVec		= DirectX::XMFLOAT4( 1.0f,	1.0f,	1.0f,	1.0f );
	m_f4LightVec[0]	= DirectX::XMFLOAT4( 1.0f,	0.5f,	1.0f,	1.0f );
	m_f4LightVec[1]	= DirectX::XMFLOAT4( -1.0f,	0.5f,	-1.0f,	1.0f );

	// Color
	#if defined( USE_SHADOW )
	m_f4MainCol		= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4LightCol[0]	= DirectX::XMFLOAT4( g_fMaxPow, g_fMaxPow, g_fMaxPow, g_fMaxSpec );
	m_f4LightCol[1]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	#elif 0
	float fPow		= g_fMaxPow / ( enLightNum + 1 ) / 3.0f;
	float fSpec		= g_fMaxSpec / ( enLightNum + 1 );
	m_f4MainCol		= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[0]	= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[1]	= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	#else
	m_f4MainCol		= DirectX::XMFLOAT4( g_fMaxPow, g_fMaxPow, g_fMaxPow, g_fMaxSpec );
	m_f4LightCol[0]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4LightCol[1]	= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	#endif
}

CLight::~CLight()
{
}
