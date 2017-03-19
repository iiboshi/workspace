/* ShadowMap class */

#include "CShadowMap.h"
#include "CModelManager.h"
#include "engine\CShader.h"
#include "engine\CDevice.h"
#include "engine\CCamera.h"
#include "object\CModel.h"

CShadowMap::CShadowMap()
	: m_pCbUpdateBuffer	( nullptr )
{
	for( int ii = 0; ii < EnShadow_Max; ii++ )
	{
		m_pVertexShader[ii]	= nullptr;
		m_pPixelShader[ii]	= nullptr;
		m_pInputLayout[ii]	= nullptr;
	}
	Init();
}

CShadowMap::~CShadowMap()
{
	I_RELEASE( m_pCbUpdateBuffer );
}

HRESULT CShadowMap::Init()
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	// Shadow Map.
	{
		CShader::Instance()->CreateVertexShader( "shadowmap", L"../shader/ShadowMap.fx" );
		CShader::Instance()->CreatePixelShader( "shadowmap", L"../shader/ShadowMap.fx" );
		m_pVertexShader[EnShadow_Map]	= CShader::Instance()->GetVertexShader( "shadowmap" );
		m_pPixelShader[EnShadow_Map]	= CShader::Instance()->GetPixelShader( "shadowmap" );
		m_pInputLayout[EnShadow_Map]	= CShader::Instance()->GetInputLayout( "shadowmap" );
	}

	// Shadow.
	{
		CShader::Instance()->CreateVertexShader( "shadow", L"../shader/Shadow.fx" );
		CShader::Instance()->CreatePixelShader( "shadow", L"../shader/Shadow.fx" );
		m_pVertexShader[EnShadow_Shadow]	= CShader::Instance()->GetVertexShader( "shadow" );
		m_pPixelShader[EnShadow_Shadow]	= CShader::Instance()->GetPixelShader( "shadow" );
		m_pInputLayout[EnShadow_Shadow]	= CShader::Instance()->GetInputLayout( "shadow" );
	}

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

void CShadowMap::Render( ID3D11DeviceContext* _pContext )
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	// ShadowMap.
	{
		m_stUpdateBuffer.m_mView = CCamera::Instance()->m_mView;
		m_stUpdateBuffer.m_mProjection = CCamera::Instance()->m_mProjection;
		_pContext->OMSetRenderTargets( 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_ShadowMap], CDevice::Instance()->m_pDepthStencilView );
		_pContext->ClearRenderTargetView( CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_ShadowMap], ClearColor );
		_pContext->ClearDepthStencilView( CDevice::Instance()->m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		_pContext->IASetInputLayout( m_pInputLayout[EnShadow_Map] );
		_pContext->VSSetShader( m_pVertexShader[EnShadow_Map], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[EnShadow_Map], NULL, 0 );
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
		ID3D11ShaderResourceView* reset[2] = { NULL, NULL };
		_pContext->PSSetShaderResources( 0, 2, reset );
	}

	// Shadow.
	{
		m_stUpdateBuffer.m_mView = CCamera::Instance()->m_mView;
		m_stUpdateBuffer.m_mProjection = CCamera::Instance()->m_mProjection;
		_pContext->OMSetRenderTargets( 1, &CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_Shadow], CDevice::Instance()->m_pDepthStencilView );
		_pContext->ClearRenderTargetView( CShader::Instance()->m_stRenderTarget.m_pRenderTargetView[CShader::enRT_Shadow], ClearColor );
		_pContext->ClearDepthStencilView( CDevice::Instance()->m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		_pContext->IASetInputLayout( m_pInputLayout[EnShadow_Shadow] );
		_pContext->VSSetShader( m_pVertexShader[EnShadow_Shadow], NULL, 0 );
		_pContext->PSSetShader( m_pPixelShader[EnShadow_Shadow], NULL, 0 );
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
		ID3D11ShaderResourceView* reset[2] = { NULL, NULL };
		_pContext->PSSetShaderResources( 0, 2, reset );
	}
}
