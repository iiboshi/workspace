/* SSAO class */

#ifndef H_CSSAO
#define H_CSSAO

#define GAUSS_WEIGHT 0.01f

#include "object\CPreRender.h"
#include "engine\CLight.h"
#include <d3d11.h>
#include <DirectXMath.h>

class CSSAO : public CPreRender
{
private:
	enum
	{
		enWeight = 8,
	};
	enum EnAO
	{
		enAO_Map,
		enAO_AO,
		enAO_BlurX,
		enAO_BlurY,
		enAO_Max
	};
	struct StUpdateBuffer
	{
		DirectX::XMMATRIX m_mView;
		DirectX::XMMATRIX m_mProjection;
		DirectX::XMMATRIX m_mWorld;
		StUpdateBuffer()
		{
			m_mWorld		= DirectX::XMMatrixIdentity();
			m_mView			= DirectX::XMMatrixIdentity();
			m_mProjection	= DirectX::XMMatrixIdentity();
		}
	};
	struct StUpdateLightBuffer
	{
		DirectX::XMMATRIX m_mAO;
		StUpdateLightBuffer()
		{
			m_mAO = DirectX::XMMatrixIdentity();
		}
	};
	struct StUpdateAOBuffer
	{
		DirectX::XMFLOAT4 m_f4ViewVec;
		DirectX::XMFLOAT4 m_f4MainCol;
		DirectX::XMFLOAT4 m_f4LightVec[CLight::enLightNum];
		DirectX::XMFLOAT4 m_f4LightCol[CLight::enLightNum];
		DirectX::XMFLOAT4 m_f4Weight[enWeight];
		StUpdateAOBuffer()
		{
			m_f4ViewVec = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			m_f4MainCol = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
			for( int ii = 0; ii < CLight::enLightNum; ii++ )
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
public:
	CSSAO();
	~CSSAO();
	HRESULT Init();
	void CalGaussWeight( float num );
	void Render( ID3D11DeviceContext* _pContext );
private:
	StUpdateBuffer			m_stUpdateBuffer;
	StUpdateLightBuffer		m_stUpdateLightBuffer;
	StUpdateAOBuffer		m_stUpdateAOBuffer;
	ID3D11VertexShader*		m_pVertexShader[enAO_Max];
	ID3D11PixelShader*		m_pPixelShader[enAO_Max];
	ID3D11InputLayout*		m_pInputLayout[enAO_Max];
	ID3D11Buffer*			m_pCbUpdateBuffer;
	ID3D11Buffer*			m_pCbUpdateLightBuffer;
	ID3D11Buffer*			m_pCbUpdateAOBuffer;
	ID3D11SamplerState*		m_pSamplerState;
	ID3D11Buffer*			m_pQuadVB;
	UINT					m_uQuadStride;
	UINT					m_uOffset;
	float					m_fTable[enWeight];
};

#endif
