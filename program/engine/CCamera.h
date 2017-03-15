/* Camera class */

#ifndef H_CCAMERA
#define H_CCAMERA

#include "object\CUpdate.h"
#include <d3d11.h>
#include <DirectXMath.h>

struct StViewProjection
{
	DirectX::XMFLOAT4 m_f4TexSize;
	DirectX::XMFLOAT4 m_fCameraPos;
	DirectX::XMMATRIX m_mView;
	DirectX::XMMATRIX m_mProjection;
	StViewProjection()
	{
		m_f4TexSize = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_fCameraPos = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		m_mView = DirectX::XMMatrixIdentity();
		m_mProjection = DirectX::XMMatrixIdentity();
	}
};

class CCamera : public CUpdate
{
I_SINGLETON( CCamera )
public:
	CCamera();
	~CCamera();
	void Update();
	HRESULT Init( ID3D11DeviceContext* _pContext );
	void Update( ID3D11DeviceContext* _pContext );
public:
	ID3D11Buffer*				m_pCbViewProjection;
	DirectX::XMMATRIX			m_mView;
	DirectX::XMMATRIX			m_mProjection;
	DirectX::XMFLOAT4			m_f4CalPos;	//< ŒvŽZ—p.
	DirectX::XMFLOAT4			m_f4CamPos;
	DirectX::XMFLOAT4			m_f4Lookat;
	float				m_fRot;
	float				m_fZoom;
	StViewProjection	m_stViewProjections;
	UINT				m_uWidth;
	UINT				m_uHeight;
	bool				m_bRotation;
};

#endif
