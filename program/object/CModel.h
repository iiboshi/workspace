/* Model class */

#ifndef H_CMODEL
#define H_CMODEL

#include <d3d11.h>
#include <DirectXMath.h>

class CModel
{
public:
	enum EnTexType
	{
		enTexType_Albedo,
		enTexType_Normal,
		enTexType_Max,
	};
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Tex;
		DirectX::XMFLOAT3 Nrm;
		DirectX::XMFLOAT3 Tan;
		DirectX::XMFLOAT4 Def;
	};
	CModel();
	virtual ~CModel();
public:
	ID3D11ShaderResourceView*	m_pShaderResourceView[enTexType_Max];
	DirectX::XMMATRIX			m_mWorldMatrix;
	ID3D11Buffer*				m_pVertexBuffer;
	ID3D11Buffer*				m_pIndexBuffer;
	UINT						m_uStride;
	UINT						m_iIndexNum;
	UINT						m_uOffset;
	D3D_PRIMITIVE_TOPOLOGY		m_eTopology;
	ID3D11RasterizerState*		m_hpRasterizerState;
	DirectX::XMFLOAT4			m_fParam0;	//< x:Normal•â³.
	DirectX::XMFLOAT4			m_fParam1;	//< x:Roughness y:Fresnel z:SSS.
	bool						m_bDraw;	//< ÅŒã‚É m_bDraw ‚ð true.
};

#endif
