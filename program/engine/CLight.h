/* Light class */

#ifndef H_CLIGHT
#define H_CLIGHT

#include "object\CUpdate.h"
#include <DirectXMath.h>

class CLight : public CUpdate
{
I_SINGLETON( CLight )
public:
	enum EnLight
	{
		enLight0,
		enLight1,
		enLightNum	= 2,
	};
	CLight();
	~CLight();
	void Update();
public:
	DirectX::XMFLOAT4	m_f4MainVec;
	DirectX::XMFLOAT4	m_f4MainCol;
	DirectX::XMFLOAT4	m_f4LightVec[enLightNum];
	DirectX::XMFLOAT4	m_f4LightCol[enLightNum];
	float				m_fRot;
};

#endif
