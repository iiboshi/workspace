/* Light class */

#include "CLight.h"

namespace
{
	const float g_fMaxPow	= 15.0f;
	const float g_fMaxSpec	= 2.0f;
}

CLight::CLight()
{
	// Light
	m_f4MainVec		= DirectX::XMFLOAT4( 1.0f,	1.0f,	1.0f,	1.0f );
	m_f4LightVec[0]	= DirectX::XMFLOAT4( 1.0f,	0.5f,	1.0f,	1.0f );
	m_f4LightVec[1]	= DirectX::XMFLOAT4( -1.0f,	0.5f,	-1.0f,	1.0f );

	// Color
	float fPow		= g_fMaxPow / ( enLightNum + 1 ) / 3.0f;
	float fSpec		= g_fMaxSpec / ( enLightNum + 1 );
	m_f4MainCol		= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[0]	= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
	m_f4LightCol[1]	= DirectX::XMFLOAT4( fPow, fPow, fPow, fSpec );
}

CLight::~CLight()
{
}
