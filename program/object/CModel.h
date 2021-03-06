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
		enTexType_Micro,
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
	ID3D11ShaderResourceView*	m_pShaderResourceViewCube;
	DirectX::XMMATRIX			m_mWorldMatrix;
	ID3D11Buffer*				m_pVertexBuffer;
	ID3D11Buffer*				m_pIndexBuffer;
	UINT						m_uStride;
	UINT						m_iIndexNum;
	UINT						m_uOffset;
	D3D_PRIMITIVE_TOPOLOGY		m_eTopology;
	ID3D11RasterizerState*		m_hpRasterizerState;
	DirectX::XMFLOAT4			m_fParam0;	//!< x:Normal補正.
	DirectX::XMFLOAT4			m_fParam1;	//!< x:Roughness y:Fresnel z:SSS.
	DirectX::XMFLOAT4			m_fParam2;	//!< x:Cube y:Microgeometry Mask.
	bool						m_bDraw;	//!< 最後に m_bDraw を true.
};

#endif
