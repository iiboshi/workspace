/* Camera class */

#ifndef H_CCAMERA
#define H_CCAMERA

#include "object\CUpdate.h"
#include <d3dx11.h>
#include <xnamath.h>

class CCamera : public CUpdate
{
I_SINGLETON( CCamera )
public:
	struct StViewProjection
	{
		XMFLOAT4 m_f4TexSize;
		XMFLOAT4 m_fCameraPos;
		XMMATRIX m_mView;
		XMMATRIX m_mProjection;
		StViewProjection()
		{
			m_f4TexSize		= XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			m_fCameraPos	= XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			m_mView			= XMMatrixIdentity();
			m_mProjection	= XMMatrixIdentity();
		}
	};
	CCamera();
	~CCamera();
	void Update();
	HRESULT Init( ID3D11DeviceContext* _pContext );
	void Update( ID3D11DeviceContext* _pContext );
public:
	ID3D11Buffer*		m_pCbViewProjection;
	XMMATRIX			m_mView;
	XMMATRIX			m_mProjection;
	XMFLOAT4			m_f4CalPos;	//< ŒvŽZ—p.
	XMFLOAT4			m_f4CamPos;
	XMFLOAT4			m_f4Lookat;
	float				m_fRot;
	float				m_fZoom;
	StViewProjection	m_stViewProjections;
	UINT				m_uWidth;
	UINT				m_uHeight;
	bool				m_bRotation;
};

#endif
