/* skin class */

#ifndef H_CSSSS
#define H_CSSSS

#include "object\CRender.h"
#include <d3d11.h>
#include <DirectXMath.h>

#define GAUSS_WEIGHT	0.01f

class CSSSS : public CRender
{
private:
	enum
	{
		enLightNum	= 2,
		enWeight = 8,
	};
	struct StUpdateBlurBuffer
	{
		DirectX::XMFLOAT4 m_f4Weight[enWeight];
		StUpdateBlurBuffer()
		{
			for( int ii = 0; ii < enWeight; ii++ )
			{
				m_f4Weight[ii] = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			}
		}
	};
public:
	enum EnShader
	{
		enShader_BlurX,
		enShader_BlurY,
		enShader_SSSS,
		enShader_Max,
	};
	CSSSS();
	~CSSSS();
	HRESULT Init();
	void CalGaussWeight( float num );
	void Render( ID3D11DeviceContext* _pContext );
private:
	ID3D11Buffer*		m_pQuadVB;
	ID3D11Buffer*		m_pCbUpdateBlurBuffer;
	UINT				m_uQuadStride;
	UINT				m_uOffset;
	ID3D11VertexShader*	m_pVertexShader[enShader_Max];
	ID3D11PixelShader*	m_pPixelShader[enShader_Max];
	ID3D11InputLayout*	m_pInputLayout[enShader_Max];
	StUpdateBlurBuffer	m_stUpdateBlurBuffer;
	float				m_fTable[enWeight];
};

#endif
