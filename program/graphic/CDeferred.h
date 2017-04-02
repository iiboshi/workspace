/* skin class */

#ifndef H_CDEFERRED
#define H_CDEFERRED

#include "object\CRender.h"
#include "engine\CLight.h"
#include <d3d11.h>
#include <DirectXMath.h>

#define GAUSS_WEIGHT	0.01f

class CDeferred : public CRender
{
public:
	enum
	{
		enWeight		= 8,
		enBeckmannSlot	= 8,
	};
	struct StUpdateBuffer
	{
		DirectX::XMFLOAT4 m_f4ViewVec;
		DirectX::XMFLOAT4 m_f4MainCol;
		DirectX::XMFLOAT4 m_f4LightVec[CLight::enLightNum];
		DirectX::XMFLOAT4 m_f4LightCol[CLight::enLightNum];
		DirectX::XMFLOAT4 m_f4AO;
		DirectX::XMFLOAT4 m_f4Weight[enWeight];
		StUpdateBuffer()
		{
			m_f4ViewVec = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			m_f4MainCol = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			for( int ii = 0; ii < CLight::enLightNum; ii++ )
			{
				m_f4LightVec[ii] = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
				m_f4LightCol[ii] = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			}
			m_f4AO		= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			for( int ii = 0; ii < enWeight; ii++ )
			{
				m_f4Weight[ii] = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			}
		}
	};
	CDeferred();
	~CDeferred();
	HRESULT Init();
	void Render( ID3D11DeviceContext* _pContext );
private:
	ID3D11Buffer*				m_pQuadVB;
	ID3D11Buffer*				m_pUpdateBuffer;
	UINT						m_uQuadStride;
	UINT						m_uOffset;
	ID3D11VertexShader*			m_pVertexShader;
	ID3D11PixelShader*			m_pPixelShader;
	ID3D11InputLayout*			m_pInputLayout;
	StUpdateBuffer				m_stUpdateBuffer;
	ID3D11ShaderResourceView*	m_pBeckmannTexture;
};

#endif
