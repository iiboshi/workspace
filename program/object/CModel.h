/* Model class */

#ifndef H_CMODEL
#define H_CMODEL

#include <d3d11.h>
#include <xnamath.h>

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
		XMFLOAT3 Pos;
		XMFLOAT2 Tex;
		XMFLOAT3 Nrm;
		XMFLOAT3 Tan;
		XMFLOAT4 Def;
	};
	CModel();
	virtual ~CModel();
public:
	ID3D11ShaderResourceView*	m_pShaderResourceView[enTexType_Max];
	XMMATRIX					m_mWorldMatrix;
	ID3D11Buffer*				m_pVertexBuffer;
	ID3D11Buffer*				m_pIndexBuffer;
	UINT						m_uStride;
	UINT						m_iIndexNum;
	UINT						m_uOffset;
	D3D_PRIMITIVE_TOPOLOGY		m_eTopology;
	ID3D11RasterizerState*		m_hpRasterizerState;
	XMFLOAT4					m_fParam0;	//< x:Normal•â³.
	XMFLOAT4					m_fParam1;	//< x:Roughness y:Fresnel z:SSS.
	bool						m_bDraw;	//< ÅŒã‚É m_bDraw ‚ð true.
};

#endif
