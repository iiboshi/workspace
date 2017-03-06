/* directx class */

#include "CWindow.h"
#include "CDevice.h"
#include "CShader.h"

CDevice::CDevice()
	: m_pd3dDevice			( nullptr )
	, m_driverType			( D3D_DRIVER_TYPE_NULL )
	, m_featureLevel		( D3D_FEATURE_LEVEL_11_0 )
	, m_pSwapChain			( nullptr )
	, m_pImmediateContext	( nullptr )
	, m_pRenderTargetView	( nullptr )
	, m_pDepthStencil		( nullptr )
	, m_pDepthStencilView	( nullptr )
	, m_pcShader			( nullptr )
	, m_uWidth				( 0 )
	, m_uHeight				( 0 )
{
	m_sampleDesc.Count		= 1;
	m_sampleDesc.Quality	= 0;
}

CDevice::~CDevice()
{
	I_DELETE( m_pcShader );
}

HRESULT CDevice::InitDevice()
{
	RECT rc;
	GetClientRect( CWindow::Instance()->GetHWnd(), &rc );
	HRESULT	hr					= S_OK;
	m_uWidth					= rc.right - rc.left;
	m_uHeight					= rc.bottom - rc.top;
	UINT	createDeviceFlags	= 0;

	#ifdef DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		m_driverType = driverTypes[driverTypeIndex];

		//デバイスの生成
		hr = D3D11CreateDevice(
					NULL,
					m_driverType,
					NULL,
					createDeviceFlags,
					featureLevels,
					numFeatureLevels,
					D3D11_SDK_VERSION,
					&m_pd3dDevice,
					&m_featureLevel,
					&m_pImmediateContext );

		if( SUCCEEDED( hr ) )
		{
			break;
		}
	}

	if( FAILED( hr ) )
	{
		return hr;
	}

	#if defined( USE_MSAA )
	for( int ii = 0; ii <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; ii++ )
	{
		UINT Quality;
		if( SUCCEEDED( m_pd3dDevice->CheckMultisampleQualityLevels( 
			DXGI_FORMAT_D24_UNORM_S8_UINT, ii, &Quality ) ) )
		{
			if( 0 < Quality )
			{
				m_sampleDesc.Count = ii;
				m_sampleDesc.Quality = Quality - 1;
			}
		}
	}
	#endif

	//スワップチェイン作成
	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width						= m_uWidth;
	hDXGISwapChainDesc.BufferDesc.Height					= m_uHeight;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	hDXGISwapChainDesc.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc							= m_sampleDesc;
	hDXGISwapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount							= 1;
	hDXGISwapChainDesc.OutputWindow							= CWindow::Instance()->GetHWnd();
	hDXGISwapChainDesc.Windowed								= TRUE;
	hDXGISwapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//インターフェース取得
	IDXGIDevice1* hpDXGI = NULL;
	I_RETURN_M( m_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice1), (void**)&hpDXGI), "QueryInterface" );

	//アダプター取得
	IDXGIAdapter* hpAdapter = NULL;
	I_RETURN_M( hpDXGI->GetAdapter( &hpAdapter ), "GetAdapter" );

	//ファクトリー取得
	IDXGIFactory* hpDXGIFactory = NULL;
	I_RETURN_M( hpAdapter->GetParent( __uuidof(IDXGIFactory), (void**)&hpDXGIFactory ), "GetParent" );

	I_RETURN_M( hpDXGIFactory->CreateSwapChain( 
		m_pd3dDevice, &hDXGISwapChainDesc, &m_pSwapChain ), "CreateSwapChain" );

	hpDXGI->Release();
	hpAdapter->Release();
	hpDXGIFactory->Release();

	// BackBuffer
	ID3D11Texture2D* pBackBuffer = NULL;
	I_RETURN( m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer ) );

	I_RETURN_FNC( m_pd3dDevice->CreateRenderTargetView( 
		pBackBuffer, NULL, &m_pRenderTargetView ), pBackBuffer->Release() );

	pBackBuffer->Release();

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory( &descDepth, sizeof(descDepth) );
	descDepth.Width					= m_uWidth;
	descDepth.Height				= m_uHeight;
	descDepth.MipLevels				= 1;
	descDepth.ArraySize				= 1;
	descDepth.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc			= m_sampleDesc;
	descDepth.Usage					= D3D11_USAGE_DEFAULT;
	descDepth.BindFlags				= D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags		= 0;
	descDepth.MiscFlags				= 0;
	I_RETURN( m_pd3dDevice->CreateTexture2D( &descDepth, NULL, &m_pDepthStencil ) );

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
	descDSV.Format				= descDepth.Format;
	descDSV.ViewDimension		= ( m_sampleDesc.Count > 1 )? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice	= 0;
	I_RETURN( m_pd3dDevice->CreateDepthStencilView( 
		m_pDepthStencil, &descDSV, &m_pDepthStencilView ) );

	m_pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView, m_pDepthStencilView );

	// Setup the viewport
	m_ViewPort.Width	= (FLOAT)m_uWidth;
	m_ViewPort.Height	= (FLOAT)m_uHeight;
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_pImmediateContext->RSSetViewports( 1, &m_ViewPort );

	// Shader
	m_pcShader = I_NEW( CShader );

	return S_OK;
}

void CDevice::CleanupDevice()
{
	if( m_pImmediateContext )
	{
		m_pImmediateContext->ClearState();
	}
	I_RELEASE( m_pSwapChain );
	I_RELEASE( m_pImmediateContext );
	I_RELEASE( m_pd3dDevice );
	I_RELEASE( m_pRenderTargetView );
	I_RELEASE( m_pDepthStencil );
	I_RELEASE( m_pDepthStencilView );
}

