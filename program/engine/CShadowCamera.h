/* ShadowCamera class */

#ifndef H_CSHADOWCAMERA
#define H_CSHADOWCAMERA

#include "CLight.h"
#include <d3d11.h>
#include <DirectXMath.h>

class CShadowCamera
{
I_SINGLETON( CShadowCamera )
public:
	CShadowCamera();
	~CShadowCamera();
	HRESULT Init();
	void Change();
private:
	struct StShadowCamera
	{
		DirectX::XMMATRIX	m_mView;
		DirectX::XMMATRIX	m_mProjection;
		DirectX::XMFLOAT4	m_f4CamPos;
		DirectX::XMFLOAT4	m_f4Lookat;
		float				m_fRot;
		float				m_fZoom;
		StShadowCamera()
		{
			m_mView			= DirectX::XMMatrixIdentity();
			m_mProjection	= DirectX::XMMatrixIdentity();
			m_f4CamPos		= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			m_f4Lookat		= DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
			m_fRot			= 0.0f;
			m_fZoom			= 1.0f;
		}
	};
public:
	StShadowCamera		m_stShwCam;
	UINT				m_uWidth;
	UINT				m_uHeight;
	bool				m_bRotation;
};

#endif
