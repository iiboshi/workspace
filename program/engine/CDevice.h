/* device class */

#ifndef H_CDDEVICE
#define H_CDDEVICE

#include <d3d11.h>

class CShader;

class CDevice
{
I_SINGLETON( CDevice )
public:
	CDevice();
	~CDevice();
	HRESULT InitDevice();
	void CleanupDevice();
public:
	ID3D11Device*			m_pd3dDevice;
	D3D_DRIVER_TYPE			m_driverType;
	D3D_FEATURE_LEVEL		m_featureLevel;
	IDXGISwapChain*			m_pSwapChain;
	ID3D11DeviceContext*	m_pImmediateContext;
	ID3D11RenderTargetView*	m_pRenderTargetView;
	ID3D11Texture2D*		m_pDepthStencil;
	ID3D11DepthStencilView*	m_pDepthStencilView;
	D3D11_VIEWPORT			m_ViewPort;
	CShader*				m_pcShader;
	DXGI_SAMPLE_DESC		m_sampleDesc;
	UINT					m_uWidth;
	UINT					m_uHeight;
};

#endif
