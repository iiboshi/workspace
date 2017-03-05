/* GBuffer class */

#ifndef H_CGBUFFER
#define H_CGBUFFER

#include "object\CPreRender.h"
#include <vector>
#include <d3d11.h>
#include <xnamath.h>

class CModel;

class CGBuffer : public CPreRender
{
public:
	struct StUpdateBuffer
	{
		XMMATRIX m_mWorld;
		XMFLOAT4 m_f4Param0;	//< x:Normal•â³.
		XMFLOAT4 m_f4Param1;	//< x:Roughness y:Fresnel z:SSS.
		StUpdateBuffer()
		{
			m_mWorld = XMMatrixIdentity();
			m_f4Param0 = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			m_f4Param1 = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
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
	ID3D11InputLayout*		m_pInputLayout;
	ID3D11Buffer*			m_pCbUpdateBuffer;
	std::vector<CModel*>	m_listModel;
};

#endif
