/* ShadowMap class */

#ifndef H_CSHADOWMAP
#define H_CSHADOWMAP

#define GAUSS_WEIGHT 0.01f

#include "object\CPreRender.h"
#include "engine\CLight.h"
#include <d3d11.h>
#include <DirectXMath.h>

class CShadowMap : public CPreRender
{
private:
	enum
	{
		enWeight = 8,
	};
	enum EnShadow
	{
		enShadow_Map,
		enShadow_Shadow,
		enShadow_BlurX,
		enShadow_BlurY,
		enShadow_Max
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
		DirectX::XMMATRIX m_mShadow;
		StUpdateLightBuffer()
		{
			m_mShadow = DirectX::XMMatrixIdentity();
		}
	};
	struct StUpdateShadowBuffer
	{
		DirectX::XMFLOAT4 m_f4ViewVec;
		DirectX::XMFLOAT4 m_f4MainCol;
		DirectX::XMFLOAT4 m_f4LightVec[CLight::enLightNum];
		DirectX::XMFLOAT4 m_f4LightCol[CLight::enLightNum];
		DirectX::XMFLOAT4 m_f4Weight[enWeight];
		StUpdateShadowBuffer()
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
	CShadowMap();
	~CShadowMap();
	HRESULT Init();
	void CalGaussWeight( float num );
	void Render( ID3D11DeviceContext* _pContext );
private:
	StUpdateBuffer			m_stUpdateBuffer;
	StUpdateLightBuffer		m_stUpdateLightBuffer;
	StUpdateShadowBuffer	m_stUpdateShadowBuffer;
	ID3D11VertexShader*		m_pVertexShader[enShadow_Max];
	ID3D11PixelShader*		m_pPixelShader[enShadow_Max];
	ID3D11InputLayout*		m_pInputLayout[enShadow_Max];
	ID3D11Buffer*			m_pCbUpdateBuffer;
	ID3D11Buffer*			m_pCbUpdateLightBuffer;
	ID3D11Buffer*			m_pCbUpdateShadowBuffer;
	ID3D11SamplerState*		m_pSamplerState;
	ID3D11Buffer*			m_pQuadVB;
	UINT					m_uQuadStride;
	UINT					m_uOffset;
	float					m_fTable[enWeight];
};

#endif
