/* skin class */

#ifndef H_CSSSS
#define H_CSSSS

#include "object\CRender.h"
#include <d3d11.h>
#include <DirectXMath.h>

#define GAUSS_WEIGHT	0.01f

class CSSSS : public CRender
{
public:
	enum
	{
		enLightNum	= 2,
		enWeight	= 8,
	};
	enum EnShader
	{
		enShader_BlurX,
		enShader_BlurY,
		enShader_SSSS,
		enShader_Max,
	};
	struct StUpdateBuffer
	{
		DirectX::XMFLOAT4 m_f4ViewVec;
		DirectX::XMFLOAT4 m_f4MainCol;
		DirectX::XMFLOAT4 m_f4LightVec[enLightNum];
		DirectX::XMFLOAT4 m_f4LightCol[enLightNum];
		DirectX::XMFLOAT4 m_f4Weight[enWeight];
		StUpdateBuffer()
		{
			m_f4ViewVec = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			m_f4MainCol = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			for( int ii = 0; ii < enLightNum; ii++ )
			{
				m_f4LightVec[ii] = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
				m_f4LightCol[ii] = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			}
			for( int ii = 0; ii < enWeight; ii++ )
			{
				m_f4Weight[ii] = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			}
		}
	};
	CSSSS();
	~CSSSS();
	HRESULT Init();
	void CalGaussWeight( float num );
	void Render( ID3D11DeviceContext* _pContext );
private:
	ID3D11Buffer*		m_pQuadVB;
	ID3D11Buffer*		m_pUpdateBuffer;
	UINT				m_uQuadStride;
	UINT				m_uOffset;
	ID3D11VertexShader*	m_pVertexShader[enShader_Max];
	ID3D11PixelShader*	m_pPixelShader[enShader_Max];
	ID3D11InputLayout*	m_pInputLayout[enShader_Max];
	StUpdateBuffer		m_stUpdateBuffer;
	float				m_fTable[enWeight];
	DirectX::XMFLOAT4			m_f4MainCol;
	DirectX::XMFLOAT4			m_f4LightVec[enLightNum];
	DirectX::XMFLOAT4			m_f4LightCol[enLightNum];
};

#endif
