/* Shadow class */

#include "CShadow.h"
#include "CModelManager.h"
#include "engine\CShader.h"
#include "engine\CDevice.h"
#include "engine\CShadowCamera.h"
#include "engine\CCamera.h"
#include "object\CModel.h"

namespace
{
	const DirectX::XMMATRIX g_mBias = DirectX::XMMATRIX(
		0.5f,  0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f,  0.0f, 1.0f, 0.0f,
		0.5f,  0.5f, 0.0f, 1.0f );
}

CShadow::CShadow()
	: m_pCbUpdateBuffer			( nullptr )
	, m_pCbUpdateLightBuffer	( nullptr )
	, m_pCbUpdateShadowBuffer	( nullptr )
	, m_pSamplerState			( nullptr )
	, m_pQuadVB					( nullptr )
	, m_uQuadStride				( 0 )
	, m_uOffset					( 0 )
{
	for( int ii = 0; ii < enShadow_Max; ii++ )
	{
		m_pVertexShader[ii]	= nullptr;
		m_pPixelShader[ii]	= nullptr;
		m_pInputLayout[ii]	= nullptr;
	}
	for( int ii = 0; ii < enWeight; ii++ )
	{
		m_fTable[ii] = 0.0f;
	}
	Init();
}

CShadow::~CShadow()
{
	I_RELEASE( m_pCbUpdateBuffer );
	I_RELEASE( m_pCbUpdateLightBuffer );
	I_RELEASE( m_pCbUpdateShadowBuffer );
	I_RELEASE( m_pSamplerState );
	I_RELEASE( m_pQuadVB );
}

HRESULT CShadow::Init()
{
	HRESULT hr = S_OK;

	CShadowCamera::Instance()->Init();

	CDevice* pcDevice = CDevice::Instance();

	// Shadow Map.
	{
		m_pVertexShader[enShadow_Map]	= CShader::Instance()->GetVertexShader( "depthmap" );
		m_pPixelShader[enShadow_Map]	= CShader::Instance()->GetPixelShader( "depthmap" );
		m_pInputLayout[enShadow_Map]	= CShader::Instance()->GetInputLayout( "depthmap" );
	}

	// Shadow.
	{
		CShader::Instance()->CreateVertexShader( "shadow", L"../shader/Shadow.fx" );
		CShader::Instance()->CreatePixelShader( "shadow", L"../shader/Shadow.fx" );
		m_pVertexShader[enShadow_Shadow]	= CShader::Instance()->GetVertexShader( "shadow" );
		m_pPixelShader[enShadow_Shadow]		= CShader::Instance()->GetPixelShader( "shadow" );
		m_pInputLayout[enShadow_Shadow]		= CShader::Instance()->GetInputLayout( "shadow" );
	}

	// Blur X.
	{
		m_pVertexShader[enShadow_BlurX]	= CShader::Instance()->GetVertexShader( "commonblurx" );
		m_pPixelShader[enShadow_BlurX]	= CShader::Instance()->GetPixelShader( "commonblurx" );
		m_pInputLayout[enShadow_BlurX]	= CShader::Instance()->GetInputLayout( "commonblurx" );
	}

	// Blur Y.
	{
		m_pVertexShader[enShadow_BlurY]	= CShader::Instance()->GetVertexShader( "commonblury" );
		m_pPixelShader[enShadow_BlurY]	= CShader::Instance()->GetPixelShader( "commonblury" );
		m_pInputLayout[enShadow_BlurY]	= CShader::Instance()->GetInputLayout( "commonblury" );
	}

	// Blur.
	{
		// 矩形描画用頂点
		struct QuadVertex
		{
			DirectX::XMFLOAT3 Pos;
			DirectX::XMFLOAT2 Uv;
		};

		// 頂点データの設定
		QuadVertex vertices[] =
		{
			{ DirectX::XMFLOAT3( 1.0f, 1.0f, 0.0f ),	DirectX::XMFLOAT2( 1.0f, 0.0f ) },
			{ DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f ),	DirectX::XMFLOAT2( 1.0f, 1.0f ) },
			{ DirectX::XMFLOAT3( -1.0f, -1.0f, 0.0f ),	DirectX::XMFLOAT2( 0.0f, 1.0f ) },

			{ DirectX::XMFLOAT3( 1.0, 1.0f, 0.0f ),		DirectX::XMFLOAT2( 1.0f, 0.0f ) },
			{ DirectX::XMFLOAT3( -1.0f, -1.0f, 0.0f ),	DirectX::XMFLOAT2( 0.0f, 1.0f ) },
			{ DirectX::XMFLOAT3( -1.0f, 1.0f, 0.0f ),	DirectX::XMFLOAT2( 0.0f, 0.0f ) },
		};

		// ストライドの設定
		m_uQuadStride = sizeof( QuadVertex );

		// 頂点バッファの設定
		D3D11_BUFFER_DESC bufDesc;
		memset( &bufDesc, 0, sizeof( bufDesc ) );
		bufDesc.Usage			= D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		bufDesc.CPUAccessFlags	= 0;
		bufDesc.ByteWidth		= sizeof( QuadVertex ) * 6;

		// サブリソースデータの設定
		D3D11_SUBRESOURCE_DATA initData;
		memset( &initData, 0, sizeof( initData ) );
		initData.pSysMem = vertices;

		// 頂点バッファ生成
		I_RETURN_M(
			pcDevice->m_pd3dDevice->CreateBuffer( &bufDesc, &initData, &m_pQuadVB ),
			"Error : ID3D11Device::CreateBuffer() Failed" );

		// Buffer.
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage			= D3D11_USAGE_DEFAULT;
		bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags	= 0;

		bd.ByteWidth		= sizeof( StUpdateShadowBuffer );
		I_RETURN( pcDevice->m_pd3dDevice->CreateBuffer( &bd, NULL, &m_pCbUpdateShadowBuffer ) );
	}

	// Buffer
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory( &bd, sizeof(bd) );
		bd.Usage			= D3D11_USAGE_DEFAULT;
		bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags	= 0;

		bd.ByteWidth		= sizeof( StUpdateBuffer );
		I_RETURN( pcDevice->m_pd3dDevice->CreateBuffer( &bd, NULL, &m_pCbUpdateBuffer ) );

		bd.ByteWidth		= sizeof( StUpdateLightBuffer );
		I_RETURN( pcDevice->m_pd3dDevice->CreateBuffer( &bd, NULL, &m_pCbUpdateLightBuffer ) );
	}

	// Sampler.
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory( &desc, sizeof( desc ) );
		desc.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.MaxAnisotropy  = 1;
		desc.MipLODBias     = 0;
		desc.MinLOD         = -FLT_MAX;
		desc.MaxLOD         = +FLT_MAX;

		// サンプラーステートを生成.
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateSamplerState( &desc, &m_pSamplerState ), "Error : ID3D11Device::CreateSamplerState() Failed." );
	}

	return hr;
}

void CShadow::CalGaussWeight( float num )
{
	float total	= 0.0f;
	
	for( int i = 0; i < enWeight; i++ )
	{
		m_fTable[i]	= pow( 3.14f, -i / ( num + GAUSS_WEIGHT ) );
		total		+= m_fTable[i] * 2.0f;
	}
	
	// 規格化
	for( int i = 0; i < enWeight; i++ )
	{
		m_fTable[i]	/= total;
	}
}

void CShadow::Render( ID3D11DeviceContext* _pContext )
{
	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	CShadowCamera::Instance()->Change();

	// ShadowMap.
	{
		m_stUpdateBuffer.m_mView = DirectX::XMMatrixTranspose( CShadowCamera::Instance()->m_stShwCam.m_mView );
		m_stUpdateBuffer.m_mProjection = DirectX::XMMatrixTranspose( CShadowCamera::Instance()->m_stShwCam.m_mProjection );
		#if defined( SHADOWMAPTEST )
		_pContext->OMSetRenderTargets( 1, &CDevice::Instance()->m_pRenderTargetView, CDevice::Instance()->m_pDepthStencilView );
		_pContext->ClearRenderTargetView( CDevice::Instance()->m_pRenderTargetView, ClearColor );
		#else
		_pContext->OMSetRenderTargets( 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_ShadowMap], CDevice::Instance()->m_pDepthStencilView );
		_pContext->ClearRenderTargetView( CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_ShadowMap], ClearColor );
		#endif
		_pContext->ClearDepthStencilView( CDevice::Instance()->m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		_pContext->IASetInputLayout( m_pInputLayout[enShadow_Map] );
		_pContext->VSSetShader( m_pVertexShader[enShadow_Map], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[enShadow_Map], NULL, 0 );
		_pContext->VSSetConstantBuffers( 0, 1, &m_pCbUpdateBuffer );
		_pContext->PSSetConstantBuffers( 0, 1, &m_pCbUpdateBuffer );
		_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );
		_pContext->RSSetViewports( 1, &CDevice::Instance()->m_ViewPort );
		for( auto obj = CModelManager::Instance()->m_listModel.begin(); obj != CModelManager::Instance()->m_listModel.end(); obj++ ) {
			if( *obj == nullptr ) {
				break;
			}
			if( !( *obj )->m_bDraw ) {
				break;
			}
			if( ( *obj )->m_hpRasterizerState != nullptr ) {
				_pContext->RSSetState( ( *obj )->m_hpRasterizerState );
			}
			m_stUpdateBuffer.m_mWorld = DirectX::XMMatrixTranspose( ( *obj )->m_mWorldMatrix );
			_pContext->UpdateSubresource( m_pCbUpdateBuffer, 0, NULL, &m_stUpdateBuffer, 0, 0 );
			_pContext->IASetVertexBuffers( 0, 1, &( *obj )->m_pVertexBuffer, &( *obj )->m_uStride, &( *obj )->m_uOffset );
			_pContext->IASetIndexBuffer( ( *obj )->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
			_pContext->IASetPrimitiveTopology( ( *obj )->m_eTopology );
			_pContext->DrawIndexed( ( *obj )->m_iIndexNum, 0, 0 );
		}
		ID3D11ShaderResourceView* reset[CShader::enRT_Max] = { 0 };
		_pContext->PSSetShaderResources( 0, CShader::enRT_Max, reset );
	}

	// Shadow.
	{
		CCamera::Instance()->Change();
		m_stUpdateBuffer.m_mView = DirectX::XMMatrixTranspose( CCamera::Instance()->m_mView );
		m_stUpdateBuffer.m_mProjection = DirectX::XMMatrixTranspose( CCamera::Instance()->m_mProjection );
		#if defined( SHADOWTEST )
		_pContext->OMSetRenderTargets( 1, &CDevice::Instance()->m_pRenderTargetView, CDevice::Instance()->m_pDepthStencilView );
		_pContext->ClearRenderTargetView( CDevice::Instance()->m_pRenderTargetView, ClearColor );
		#else
		_pContext->OMSetRenderTargets( 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_Shadow], CDevice::Instance()->m_pDepthStencilView );
		_pContext->ClearRenderTargetView( CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_Shadow], ClearColor );
		#endif
		_pContext->ClearDepthStencilView( CDevice::Instance()->m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		_pContext->IASetInputLayout( m_pInputLayout[enShadow_Shadow] );
		_pContext->VSSetShader( m_pVertexShader[enShadow_Shadow], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[enShadow_Shadow], NULL, 0 );
		_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );
		_pContext->RSSetViewports( 1, &CDevice::Instance()->m_ViewPort );
		#if defined( USE_MSAA )
		if( CDevice::Instance()->m_sampleDesc.Count > 1 ) {
			_pContext->ResolveSubresource( 
				CShader::Instance()->m_stRenderTarget.m_pResolveTexture[CShader::enRT_ShadowMap], 0, 
				CShader::Instance()->m_stRenderTarget.m_pTexture[CShader::enRT_ShadowMap], 0, 
				CShader::Instance()->m_stRenderTarget.m_resolveFormat );
			_pContext->PSSetShaderResources( 0, 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTextureSRV[CShader::enRT_ShadowMap] );
		} else
		#endif
		{
			_pContext->PSSetShaderResources( 0, 1, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_ShadowMap] );
		}
		m_stUpdateLightBuffer.m_mShadow = DirectX::XMMatrixTranspose( 
			CShadowCamera::Instance()->m_stShwCam.m_mView * CShadowCamera::Instance()->m_stShwCam.m_mProjection * g_mBias );

		_pContext->UpdateSubresource( m_pCbUpdateLightBuffer, 0, NULL, &m_stUpdateLightBuffer, 0, 0 );
		_pContext->VSSetConstantBuffers( 1, 1, &m_pCbUpdateLightBuffer );
		_pContext->PSSetConstantBuffers( 1, 1, &m_pCbUpdateLightBuffer );
		for( auto obj = CModelManager::Instance()->m_listModel.begin(); obj != CModelManager::Instance()->m_listModel.end(); obj++ ) {
			if( *obj == nullptr ) {
				break;
			}
			if( !( *obj )->m_bDraw ) {
				break;
			}
			if( ( *obj )->m_hpRasterizerState != nullptr ) {
				_pContext->RSSetState( ( *obj )->m_hpRasterizerState );
			}
			m_stUpdateBuffer.m_mWorld = DirectX::XMMatrixTranspose( ( *obj )->m_mWorldMatrix );
			_pContext->UpdateSubresource( m_pCbUpdateBuffer, 0, NULL, &m_stUpdateBuffer, 0, 0 );
			_pContext->VSSetConstantBuffers( 0, 1, &m_pCbUpdateBuffer );
			_pContext->PSSetConstantBuffers( 0, 1, &m_pCbUpdateBuffer );
			_pContext->IASetVertexBuffers( 0, 1, &( *obj )->m_pVertexBuffer, &( *obj )->m_uStride, &( *obj )->m_uOffset );
			_pContext->IASetIndexBuffer( ( *obj )->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
			_pContext->IASetPrimitiveTopology( ( *obj )->m_eTopology );
			_pContext->DrawIndexed( ( *obj )->m_iIndexNum, 0, 0 );
		}
		ID3D11ShaderResourceView* reset[CShader::enRT_Max] = { 0 };
		_pContext->PSSetShaderResources( 0, CShader::enRT_Max, reset );
	}

	// Blur 共通.
	{
		CCamera::Instance()->Update( _pContext );

		// ガウス defo 5
		CalGaussWeight( 5 );
		for( int ii = 0; ii < enWeight; ii++ )
		{
			m_stUpdateShadowBuffer.m_f4Weight[ii] = DirectX::XMFLOAT4( m_fTable[ii], m_fTable[ii], m_fTable[ii], 1.0f );
		}
		_pContext->UpdateSubresource( m_pCbUpdateShadowBuffer, 0, NULL, &m_stUpdateShadowBuffer, 0, 0 );
	}

	// Blur X
	{
		// デフォルトのレンダーターゲットビューに切り替え
		_pContext->OMSetRenderTargets( 1, 
			&CShader::Instance()->m_pWorkRenderTargetView[CShader::enWorkRT0], NULL );
		// シェーダを設定
		_pContext->VSSetShader( m_pVertexShader[enShadow_BlurX], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[enShadow_BlurX], NULL, 0 );
		// Buffer.
		_pContext->VSSetConstantBuffers( 1, 1, &m_pCbUpdateShadowBuffer );
		_pContext->PSSetConstantBuffers( 1, 1, &m_pCbUpdateShadowBuffer );
		// レンダリングテクスチャを設定
		#if defined( USE_MSAA )
		if( CDevice::Instance()->m_sampleDesc.Count > 1 )
		{
			_pContext->ResolveSubresource( 
				CShader::Instance()->m_stRenderTarget.m_pResolveTexture[CShader::enRT_Shadow], 0, 
				CShader::Instance()->m_stRenderTarget.m_pTexture[CShader::enRT_Shadow], 0, 
				CShader::Instance()->m_stRenderTarget.m_resolveFormat );
			_pContext->PSSetShaderResources( 0, 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTextureSRV[CShader::enRT_Shadow] );
		}
		else
		#endif
		{
			_pContext->PSSetShaderResources( 0, 1, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_Shadow] );
		}
		// サンプラーステートの設定
		_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );
		// レイアウトの切り替え（VSへ送るデータサイズの変更）
		_pContext->IASetInputLayout( m_pInputLayout[enShadow_BlurX] );
		// 頂点バッファを設定
		_pContext->IASetVertexBuffers( 0, 1, &m_pQuadVB, &m_uQuadStride, &m_uOffset );
		_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		// Setup the viewport
		_pContext->RSSetViewports( 1, &CDevice::Instance()->m_ViewPort );
		// 矩形描画
		_pContext->Draw( 6, 0 );
		// 片づけ
		ID3D11ShaderResourceView* reset[CShader::enRT_Max] = { 0 };
		_pContext->PSSetShaderResources( 0, CShader::enRT_Max, reset );
	}

	// Blur Y
	{
		// デフォルトのレンダーターゲットビューに切り替え
		#if defined( SHADOWBLURTEST )
		_pContext->OMSetRenderTargets( 1, &CDevice::Instance()->m_pRenderTargetView, NULL );
		_pContext->ClearRenderTargetView( CDevice::Instance()->m_pRenderTargetView, ClearColor );
		#else
		_pContext->OMSetRenderTargets( 1, 
			&CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_Shadow] , NULL );
		#endif
		// シェーダを設定
		_pContext->VSSetShader( m_pVertexShader[enShadow_BlurY], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[enShadow_BlurY], NULL, 0 );
		// Buffer.
		_pContext->VSSetConstantBuffers( 1, 1, &m_pCbUpdateShadowBuffer );
		_pContext->PSSetConstantBuffers( 1, 1, &m_pCbUpdateShadowBuffer );
		// レンダリングテクスチャを設定
		_pContext->PSSetShaderResources( 0, 1, &CShader::Instance()->m_pWorkShaderResourceView[CShader::enWorkRT0] );
		// サンプラーステートの設定
		_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );
		// レイアウトの切り替え（VSへ送るデータサイズの変更）
		_pContext->IASetInputLayout( m_pInputLayout[enShadow_BlurY] );
		// 頂点バッファを設定
		_pContext->IASetVertexBuffers( 0, 1, &m_pQuadVB, &m_uQuadStride, &m_uOffset );
		_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		// Setup the viewport
		_pContext->RSSetViewports( 1, &CDevice::Instance()->m_ViewPort );
		// 矩形描画
		_pContext->Draw( 6, 0 );
		// 片づけ
		ID3D11ShaderResourceView* reset[CShader::enRT_Max] = { 0 };
		_pContext->PSSetShaderResources( 0, CShader::enRT_SSNum, reset );
	}
}
