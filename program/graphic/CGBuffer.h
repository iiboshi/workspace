/* GBuffer class */

#ifndef H_CGBUFFER
#define H_CGBUFFER

#include "object\CPreRender.h"
#include <d3d11.h>
#include <DirectXMath.h>

class CModel;

class CGBuffer : public CPreRender
{
public:
	struct StUpdateBuffer
	{
		DirectX::XMMATRIX m_mWorld;
		DirectX::XMFLOAT4 m_f4Param0;	//!< x:Normal•â³.
		DirectX::XMFLOAT4 m_f4Param1;	//!< x:Roughness y:Fresnel z:SSS.
		DirectX::XMFLOAT4 m_f4Param2;	//!< x:Cube.
		StUpdateBuffer()
		{
			m_mWorld = DirectX::XMMatrixIdentity();
			m_f4Param0 = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			m_f4Param1 = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			m_f4Param2 = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		}
	};
	CGBuffer();
	~CGBuffer();
	HRESULT Init();
	void Render( ID3D11DeviceContext* _pContext );
private:
	StUpdateBuffer			m_stUpdateBuffer;
	ID3D11VertexShader*		m_pVertexShader;
	ID3D11PixelShader*		m_pPixelShader;
	ID3D11HullShader*		m_pHulllShader;
	ID3D11DomainShader*		m_pDomainShader;
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pCbUpdateBuffer;
};

#endif
