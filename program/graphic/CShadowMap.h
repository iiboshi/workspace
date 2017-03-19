/* ShadowMap class */

#ifndef H_CSHADOWMAP
#define H_CSHADOWMAP

#include "object\CPreRender.h"
#include <d3d11.h>
#include <DirectXMath.h>

class CShadowMap : public CPreRender
{
private:
	enum EnShadow
	{
		EnShadow_Map,
		EnShadow_Shadow,
		EnShadow_Max
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
public:
	CShadowMap();
	~CShadowMap();
	HRESULT Init();
	void Render( ID3D11DeviceContext* _pContext );
private:
	StUpdateBuffer			m_stUpdateBuffer;
	StUpdateLightBuffer		m_stUpdateLightBuffer;
	ID3D11VertexShader*		m_pVertexShader[EnShadow_Max];
	ID3D11PixelShader*		m_pPixelShader[EnShadow_Max];
	ID3D11InputLayout*		m_pInputLayout[EnShadow_Max];
	ID3D11Buffer*			m_pCbUpdateBuffer;
	ID3D11Buffer*			m_pCbUpdateLightBuffer;
	ID3D11SamplerState*		m_pSamplerState;
};

#endif
