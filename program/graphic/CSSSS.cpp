/* skin class */

#include "CSSSS.h"
#include "engine\CCamera.h"
#include "engine\CShader.h"
#include "engine\CDevice.h"

CSSSS::CSSSS()
	: m_pQuadVB			( nullptr )
	, m_uQuadStride		( 0 )
	, m_uOffset			( 0 )
{
	for( int ii = 0; ii < enShader_Max; ii++ )
	{
		m_pVertexShader[ii]	= nullptr;
		m_pPixelShader[ii]	= nullptr;
		m_pInputLayout[ii]	= nullptr;
	}
	Init();
}

CSSSS::~CSSSS()
{
	I_RELEASE( m_pQuadVB );
}

HRESULT CSSSS::Init()
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	CShader::Instance()->CreateVertexShader( "blurx", L"../shader/SSSSBlurX.fx" );
	CShader::Instance()->CreatePixelShader( "blurx", L"../shader/SSSSBlurX.fx" );
	CShader::Instance()->CreateVertexShader( "blury", L"../shader/SSSSBlurY.fx" );
	CShader::Instance()->CreatePixelShader( "blury", L"../shader/SSSSBlurY.fx" );
	CShader::Instance()->CreateVertexShader( "ssss", L"../shader/SSSSFinal.fx" );
	CShader::Instance()->CreatePixelShader( "ssss", L"../shader/SSSSFinal.fx" );

	// 矩形描画用頂点
	struct QuadVertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 Uv;
	};

	// 頂点データの設定
	QuadVertex vertices[] =
	{
		{ DirectX::XMFLOAT3( 1.0f, 1.0f, 0.0f ),		DirectX::XMFLOAT2( 1.0f, 0.0f ) },
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

	// Set
	m_pVertexShader[enShader_BlurX]	= CShader::Instance()->GetVertexShader( "blurx" );
	m_pPixelShader[enShader_BlurX]	= CShader::Instance()->GetPixelShader( "blurx" );
	m_pInputLayout[enShader_BlurX]	= CShader::Instance()->GetInputLayout( "blurx" );
	m_pVertexShader[enShader_BlurY]	= CShader::Instance()->GetVertexShader( "blury" );
	m_pPixelShader[enShader_BlurY]	= CShader::Instance()->GetPixelShader( "blury" );
	m_pInputLayout[enShader_BlurY]	= CShader::Instance()->GetInputLayout( "blury" );
	m_pVertexShader[enShader_SSSS]	= CShader::Instance()->GetVertexShader( "ssss" );
	m_pPixelShader[enShader_SSSS]	= CShader::Instance()->GetPixelShader( "ssss" );
	m_pInputLayout[enShader_SSSS]	= CShader::Instance()->GetInputLayout( "ssss" );


	// Light
	m_f4MainCol = DirectX::XMFLOAT4( 4.0f, 4.0f, 4.0f, 1.0f );
	m_f4LightVec[0] = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_f4LightVec[1] = DirectX::XMFLOAT4( -1.0f, 1.0f, -1.0f, 1.0f );
	m_f4LightCol[0] = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4LightCol[1] = DirectX::XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );

	return hr;
}

void CSSSS::Render( ID3D11DeviceContext* _pContext )
{
	CCamera::Instance()->Update( _pContext );


	// Blur X
	{
		// デフォルトのレンダーターゲットビューに切り替え
		_pContext->OMSetRenderTargets( 1, 
			&CShader::Instance()->m_pWorkRenderTargetView[CShader::enWorkRT0], NULL );
		// シェーダを設定
		_pContext->VSSetShader( m_pVertexShader[enShader_BlurX], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[enShader_BlurX], NULL, 0 );
		// レンダリングテクスチャを設定
		_pContext->PSSetShaderResources( 0, CShader::enRT_SSNum, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_SSStart] );
		// サンプラーステートの設定
		_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );
		// レイアウトの切り替え（VSへ送るデータサイズの変更）
		_pContext->IASetInputLayout( m_pInputLayout[enShader_BlurX] );
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
		_pContext->OMSetRenderTargets( 1, 
			&CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_SSDest], NULL );
		// シェーダを設定
		_pContext->VSSetShader( m_pVertexShader[enShader_BlurY], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[enShader_BlurY], NULL, 0 );
		// レンダリングテクスチャを設定
		_pContext->PSSetShaderResources( 0, 1, &CShader::Instance()->m_pWorkShaderResourceView[CShader::enWorkRT0] );
		_pContext->PSSetShaderResources( 1, 1, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_SSColor] );
		_pContext->PSSetShaderResources( 2, 1, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_SSSpec] );
		_pContext->PSSetShaderResources( 3, 1, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_SSParam] );
		// サンプラーステートの設定
		_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );
		// レイアウトの切り替え（VSへ送るデータサイズの変更）
		_pContext->IASetInputLayout( m_pInputLayout[enShader_BlurY] );
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
	// SSSS Final
	{
		// デフォルトのレンダーターゲットビューに切り替え
		_pContext->OMSetRenderTargets( 1, &CDevice::Instance()->m_pRenderTargetView, NULL );
		// シェーダを設定
		_pContext->VSSetShader( m_pVertexShader[enShader_SSSS], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[enShader_SSSS], NULL, 0 );
		// レンダリングテクスチャを設定
		_pContext->PSSetShaderResources( 0, CShader::enRT_SSNum, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_SSStart] );
		// サンプラーステートの設定
		_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );
		// レイアウトの切り替え（VSへ送るデータサイズの変更）
		_pContext->IASetInputLayout( m_pInputLayout[enShader_SSSS] );
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
}
