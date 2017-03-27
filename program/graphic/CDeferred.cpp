/* skin class */

#include "CDeferred.h"
#include "engine\CCamera.h"
#include "engine\CShader.h"
#include "engine\CDevice.h"

CDeferred::CDeferred()
	: m_pQuadVB				( nullptr )
	, m_pUpdateBuffer		( nullptr )
	, m_uQuadStride			( 0 )
	, m_uOffset				( 0 )
	, m_pVertexShader		( nullptr )
	, m_pPixelShader		( nullptr )
	, m_pInputLayout		( nullptr )
{
	for( int ii = 0; ii < enWeight; ii++ )
	{
		m_fTable[ii] = 0.0f;
	}
	Init();
}

CDeferred::~CDeferred()
{
	I_RELEASE( m_pQuadVB );
	I_RELEASE( m_pUpdateBuffer );
}

HRESULT CDeferred::Init()
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	CShader::Instance()->CreateVertexShader( "deferred", L"../shader/Deferred.fx" );
	CShader::Instance()->CreatePixelShader( "deferred", L"../shader/Deferred.fx" );

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

	// Set
	m_pVertexShader = CShader::Instance()->GetVertexShader( "deferred" );
	m_pPixelShader	= CShader::Instance()->GetPixelShader( "deferred" );
	m_pInputLayout	= CShader::Instance()->GetInputLayout( "deferred" );

	// Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags	= 0;

	bd.ByteWidth		= sizeof( StUpdateBuffer );
	I_RETURN( pcDevice->m_pd3dDevice->CreateBuffer( &bd, NULL, &m_pUpdateBuffer ) );

	return hr;
}

void CDeferred::CalGaussWeight( float num )
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

void CDeferred::Render( ID3D11DeviceContext* _pContext )
{
	CCamera::Instance()->Update( _pContext );

	// ガウス defo 5
	CalGaussWeight( 5 );
	for( int ii = 0; ii < enWeight; ii++ )
	{
		m_stUpdateBuffer.m_f4Weight[ii] = DirectX::XMFLOAT4( m_fTable[ii], m_fTable[ii], m_fTable[ii], 1.0f );
	}

	// Light
	m_stUpdateBuffer.m_f4ViewVec = CCamera::Instance()->m_f4CamPos;
	m_stUpdateBuffer.m_f4MainCol = CLight::Instance()->m_f4MainCol;
	for( int ii = 0; ii < CLight::enLightNum; ii++ )
	{
		m_stUpdateBuffer.m_f4LightVec[ii] = CLight::Instance()->m_f4LightVec[ii];
		m_stUpdateBuffer.m_f4LightCol[ii] = CLight::Instance()->m_f4LightCol[ii];
	}
	m_stUpdateBuffer.m_f4AO = CLight::Instance()->m_f4AO;

	// 自前のレンダーターゲットビューに切り替え
	_pContext->OMSetRenderTargets( 
		CShader::enRT_SSNum, 
		&CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_SSStart], 
		NULL );

	// シェーダを設定
	_pContext->VSSetShader( m_pVertexShader, NULL, 0 );
	_pContext->PSSetShader( m_pPixelShader, NULL, 0 );

	#if !defined( USE_SHADOW )
	float ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	_pContext->ClearRenderTargetView( CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_Shadow], ClearColor );
	#endif

	// レンダリングテクスチャを設定
	#if defined( USE_MSAA )
	if( CDevice::Instance()->m_sampleDesc.Count > 1 )
	{
		for( int ii = CShader::enRT_GBStart; ii < CShader::enRT_GBEnd; ii++ )
		{
			_pContext->ResolveSubresource( 
				CShader::Instance()->m_stRenderTarget.m_pResolveTexture[ii], 0, 
				CShader::Instance()->m_stRenderTarget.m_pTexture[ii], 0, 
				CShader::Instance()->m_stRenderTarget.m_resolveFormat );
		}
		_pContext->ResolveSubresource( 
			CShader::Instance()->m_stRenderTarget.m_pResolveTexture[CShader::enRT_Shadow], 0, 
			CShader::Instance()->m_stRenderTarget.m_pTexture[CShader::enRT_Shadow], 0, 
			CShader::Instance()->m_stRenderTarget.m_resolveFormat );
		_pContext->ResolveSubresource( 
			CShader::Instance()->m_stRenderTarget.m_pResolveTexture[CShader::enRT_AO], 0, 
			CShader::Instance()->m_stRenderTarget.m_pTexture[CShader::enRT_AO], 0, 
			CShader::Instance()->m_stRenderTarget.m_resolveFormat );
		_pContext->PSSetShaderResources( 0, CShader::enRT_GBNum, &CShader::Instance()->m_stRenderTarget.m_pRenderTextureSRV[CShader::enRT_GBStart] );
		_pContext->PSSetShaderResources( CShader::enRT_GBNum, 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTextureSRV[CShader::enRT_Shadow] );
		_pContext->PSSetShaderResources( CShader::enRT_GBNum + 1, 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTextureSRV[CShader::enRT_AO] );
	}
	else
	#endif
	{
		_pContext->PSSetShaderResources( 0, CShader::enRT_GBNum, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_GBStart] );
		_pContext->PSSetShaderResources( CShader::enRT_GBNum, 1, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_Shadow] );
		_pContext->PSSetShaderResources( CShader::enRT_GBNum + 1, 1, &CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_AO] );
	}

	// サンプラーステートの設定
	_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );

	// レイアウトの切り替え（VSへ送るデータサイズの変更）
	_pContext->IASetInputLayout( m_pInputLayout );

	// 頂点バッファを設定
	_pContext->IASetVertexBuffers( 0, 1, &m_pQuadVB, &m_uQuadStride, &m_uOffset );
	_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Const Buffer
	_pContext->VSSetConstantBuffers( 1, 1, &m_pUpdateBuffer );
	_pContext->PSSetConstantBuffers( 1, 1, &m_pUpdateBuffer );

	// Update
	_pContext->UpdateSubresource( 
		m_pUpdateBuffer, 0, NULL, 
		&m_stUpdateBuffer, 0, 0 );

	// Setup the viewport
	_pContext->RSSetViewports( 1, &CDevice::Instance()->m_ViewPort );

	// 矩形描画
	_pContext->Draw( 6, 0 );

	// 片づけ
	ID3D11ShaderResourceView* reset[CShader::enRT_Max] = { 0 };
	_pContext->PSSetShaderResources( 0, CShader::enRT_Max, reset );
}
