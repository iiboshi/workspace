/* GBuffer class */

#include "CGBuffer.h"
#include "CModelManager.h"
#include "engine\CShader.h"
#include "engine\CDevice.h"
#include "engine\CCamera.h"
#include "object\CModel.h"

// #define USE_TESSELLATION

CGBuffer::CGBuffer()
	: m_pVertexShader	( nullptr )
	, m_pPixelShader	( nullptr )
	, m_pInputLayout	( nullptr )
	, m_pCbUpdateBuffer	( nullptr )
{
	Init();
}

CGBuffer::~CGBuffer()
{
	I_RELEASE( m_pCbUpdateBuffer );
}

HRESULT CGBuffer::Init()
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	CShader::Instance()->CreateShader( "gbuffer",SHADE_RPATH( GBuffer ) );
#if defined( USE_TESSELLATION )
	CShader::Instance()->CreateTessellationShader( "gbuffer", SHADE_RPATH( GBuffer ) );
#endif

	// Set
	m_pVertexShader	= CShader::Instance()->GetVertexShader( "gbuffer" );
	m_pPixelShader	= CShader::Instance()->GetPixelShader( "gbuffer" );
	m_pHulllShader	= CShader::Instance()->GetHullShader( "gbuffer" );
	m_pDomainShader	= CShader::Instance()->GetDomainShader( "gbuffer" );
	m_pInputLayout	= CShader::Instance()->GetInputLayout( "gbuffer" );

	// Buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof(bd) );
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags	= 0;

	bd.ByteWidth		= sizeof( StUpdateBuffer );
	I_RETURN( pcDevice->m_pd3dDevice->CreateBuffer( &bd, NULL, &m_pCbUpdateBuffer ) );

	return hr;
}

void CGBuffer::Render( ID3D11DeviceContext* _pContext )
{
	CCamera::Instance()->Update( _pContext );

	float ClearColor[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

	// 自前のレンダーターゲットビューに切り替え
	_pContext->OMSetRenderTargets( 
		CShader::enRT_GBEnd, 
		&CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_GBStart], 
		CDevice::Instance()->m_pDepthStencilView );

	for(int ii = CShader::enRT_GBStart; ii < CShader::enRT_GBEnd; ii++){
		_pContext->ClearRenderTargetView( 
			CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[ii], 
			ClearColor );
	}
	_pContext->ClearDepthStencilView( 
		CDevice::Instance()->m_pDepthStencilView, 
		D3D11_CLEAR_DEPTH, 1.0f, 0 );

	// レイアウトの切り替え（VSへ送るデータサイズの変更）
	_pContext->IASetInputLayout( m_pInputLayout );

	// Shader
	_pContext->VSSetShader( m_pVertexShader, NULL, 0 );
#if defined( USE_TESSELLATION )
	_pContext->HSSetShader( m_pHulllShader, NULL, 0 );
	_pContext->DSSetShader( m_pDomainShader, NULL, 0 );
#endif
	_pContext->PSSetShader( m_pPixelShader, NULL, 0 );

	// Const Buffer
	_pContext->VSSetConstantBuffers( 1, 1, &m_pCbUpdateBuffer );
#if defined( USE_TESSELLATION )
	_pContext->HSSetConstantBuffers( 1, 1, &m_pCbUpdateBuffer );
	_pContext->DSSetConstantBuffers( 1, 1, &m_pCbUpdateBuffer );
#endif
	_pContext->PSSetConstantBuffers( 1, 1, &m_pCbUpdateBuffer );

	// サンプラーステートの設定
	_pContext->PSSetSamplers( 0, CShader::enState_Max, CShader::Instance()->m_pSamplerState );

	// Setup the viewport
	_pContext->RSSetViewports( 1, &CDevice::Instance()->m_ViewPort );

	// Draw
	for( auto obj = CModelManager::Instance()->m_listModel.begin(); obj != CModelManager::Instance()->m_listModel.end(); obj++ )
	{
		if( *obj == nullptr )
		{
			break;
		}

		if( !( *obj )->m_bDraw )
		{
			break;
		}

		// Rasterizer
		if( ( *obj )->m_hpRasterizerState != nullptr )
		{
			_pContext->RSSetState( ( *obj )->m_hpRasterizerState );
		}

		// Texture
		_pContext->PSSetShaderResources( 0, CModel::enTexType_Max, ( *obj )->m_pShaderResourceView );
		if( ( *obj )->m_pShaderResourceViewCube != nullptr )
		{
			_pContext->PSSetShaderResources( CModel::enTexType_Max, 1, &( *obj )->m_pShaderResourceViewCube );
		}

		// World Matrix
		m_stUpdateBuffer.m_mWorld = DirectX::XMMatrixTranspose( ( *obj )->m_mWorldMatrix );

		// Param.
		m_stUpdateBuffer.m_f4Param0 = ( *obj )->m_fParam0;
		m_stUpdateBuffer.m_f4Param1 = ( *obj )->m_fParam1;
		m_stUpdateBuffer.m_f4Param2 = ( *obj )->m_fParam2;

		// Update
		_pContext->UpdateSubresource( 
			m_pCbUpdateBuffer, 0, NULL, 
			&m_stUpdateBuffer, 0, 0 );

		// Set vertex buffer
		_pContext->IASetVertexBuffers( 
			0, 1, &( *obj )->m_pVertexBuffer, &( *obj )->m_uStride, &( *obj )->m_uOffset );

		// Set index buffer
		_pContext->IASetIndexBuffer( ( *obj )->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

		// Set primitive topology
#if defined( USE_TESSELLATION )
		_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
#else
		 _pContext->IASetPrimitiveTopology( ( *obj )->m_eTopology );
#endif

		// 描画
		_pContext->DrawIndexed( ( *obj )->m_iIndexNum, 0, 0 );
	}

#if defined( USE_TESSELLATION )
	//! 解除.
	ID3D11Buffer* pBuff = NULL;
	_pContext->HSSetConstantBuffers( 1, 1, &pBuff );
	_pContext->DSSetConstantBuffers( 1, 1, &pBuff );
	_pContext->HSSetShader( NULL, NULL, 0 );
	_pContext->DSSetShader( NULL, NULL, 0 );
#endif

	// clear
	ID3D11ShaderResourceView* reset[CShader::enRT_Max] = { 0 };
	_pContext->PSSetShaderResources( 0, CShader::enRT_Max, reset );
}
