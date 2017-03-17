/* Light class */

#ifndef H_CLIGHT
#define H_CLIGHT

#include <DirectXMath.h>

class CLight
{
I_SINGLETON( CLight )
public:
	enum
	{
		enLightNum	= 2,
	};
	CLight();
	~CLight();
public:
	DirectX::XMFLOAT4	m_f4MainVec;
	DirectX::XMFLOAT4	m_f4MainCol;
	DirectX::XMFLOAT4	m_f4LightVec[enLightNum];
	DirectX::XMFLOAT4	m_f4LightCol[enLightNum];
};

#endif
