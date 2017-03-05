/* skin class */

#include "CSSSS.h"
#include "engine\CCamera.h"
#include "engine\CShader.h"
#include "engine\CDevice.h"

CSSSS::CSSSS()
	: m_pQuadVB			( nullptr )
	, m_pUpdateBuffer	( nullptr )
	, m_uQuadStride		( 0 )
	, m_uOffset			( 0 )
	, m_pVertexShader	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pInputLayout	( nullptr )
{
	for( int ii = 0; ii < enWeight; ii++ )
	{
		m_fTable[ii] = 0.0f;
	}
	Init();
}

CSSSS::~CSSSS()
{
	I_RELEASE( m_pQuadVB );
	I_RELEASE( m_pUpdateBuffer );
}

HRESULT CSSSS::Init()
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	CShader::Instance()->CreateVertexShader( "ssss", "../shader/SSSS.fx" );
	CShader::Instance()->CreatePixelShader( "ssss", "../shader/SSSS.fx" );

	// 矩形描画用頂点
	struct QuadVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Uv;
	};

	// 頂点データの設定
	QuadVertex vertices[] =
	{
		{ XMFLOAT3( 1.0f, 1.0f, 0.0f ),		XMFLOAT2( 1.0f, 0.0f ) },
		{ XMFLOAT3( 1.0f, -1.0f, 0.0f ),	XMFLOAT2( 1.0f, 1.0f ) },
		{ XMFLOAT3( -1.0f, -1.0f, 0.0f ),	XMFLOAT2( 0.0f, 1.0f ) },

		{ XMFLOAT3( 1.0, 1.0f, 0.0f ),		XMFLOAT2( 1.0f, 0.0f ) },
		{ XMFLOAT3( -1.0f, -1.0f, 0.0f ),	XMFLOAT2( 0.0f, 1.0f ) },
		{ XMFLOAT3( -1.0f, 1.0f, 0.0f ),	XMFLOAT2( 0.0f, 0.0f ) },
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
	m_pVertexShader = CShader::Instance()->GetVertexShader( "ssss" );
	m_pPixelShader	= CShader::Instance()->GetPixelShader( "ssss" );
	m_pInputLayout	= CShader::Instance()->GetInputLayout( "ssss" );

	// Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags	= 0;

	bd.ByteWidth		= sizeof( StUpdateBuffer );
	I_RETURN( pcDevice->m_pd3dDevice->CreateBuffer( &bd, NULL, &m_pUpdateBuffer ) );

	// Light
	m_f4MainCol = XMFLOAT4( 4.0f, 4.0f, 4.0f, 1.0f );
	m_f4LightVec[0] = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	m_f4LightVec[1] = XMFLOAT4( -1.0f, 1.0f, -1.0f, 1.0f );
	m_f4LightCol[0] = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_f4LightCol[1] = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );

	return hr;
}

void CSSSS::CalGaussWeight( float num )
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

void CSSSS::Render( ID3D11DeviceContext* _pContext )
{
	CCamera::Instance()->Update( _pContext );

	// ガウス defo 5 カメラ距離によって変化
	float workLong	=	abs( CCamera::Instance()->m_f4Lookat.x - CCamera::Instance()->m_f4Lookat.x );
	workLong		+=	abs( CCamera::Instance()->m_f4Lookat.y - CCamera::Instance()->m_f4Lookat.y );
	workLong		+=	abs( CCamera::Instance()->m_f4Lookat.z - CCamera::Instance()->m_f4Lookat.z );
	CalGaussWeight( 5 );

	// Light
	m_stUpdateBuffer.m_f4ViewVec = CCamera::Instance()->m_f4CamPos;
	m_stUpdateBuffer.m_f4MainCol = m_f4MainCol;
	for( int ii = 0; ii < enLightNum; ii++ )
	{
		m_stUpdateBuffer.m_f4LightVec[ii] = m_f4LightVec[ii];
		m_stUpdateBuffer.m_f4LightCol[ii] = m_f4LightCol[ii];
	}

	for( int ii = 0; ii < enWeight; ii++ )
	{
		m_stUpdateBuffer.m_f4Weight[ii] = XMFLOAT4( m_fTable[ii], m_fTable[ii], m_fTable[ii], 1.0f );
	}

	// デフォルトのレンダーターゲットビューに切り替え
	_pContext->OMSetRenderTargets(
		1, &CDevice::Instance()->m_pRenderTargetView,
		NULL );

	// シェーダを設定
	_pContext->VSSetShader( m_pVertexShader, NULL, 0 );
	_pContext->PSSetShader( m_pPixelShader, NULL, 0 );

	// レンダリングテクスチャを設定
	_pContext->PSSetShaderResources( 
		0, CShader::enRT_SSNum, 
		&CShader::Instance()->m_stRenderTarget.m_pShaderResourceView[CShader::enRT_SSStart] );

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
	ID3D11ShaderResourceView* reset[CShader::enRT_SSNum] = { NULL, NULL, NULL, NULL };
	_pContext->PSSetShaderResources( 0, CShader::enRT_SSNum, reset );
}
