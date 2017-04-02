/* Model class */

#include "CModel.h"
#include "engine\CDevice.h"

CModel::CModel()
	: m_pVertexBuffer		( nullptr )
	, m_pIndexBuffer		( nullptr )
	, m_uStride				( 0 )
	, m_iIndexNum			( 0 )
	, m_uOffset				( 0 )
	, m_eTopology			( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
	, m_hpRasterizerState	( nullptr )
	, m_bDraw				( true )
{
	for( int ii = 0; ii < enTexType_Max; ii++ )
	{
		m_pShaderResourceView[ii] = nullptr;
	}
	m_mWorldMatrix = DirectX::XMMatrixIdentity();
	m_fParam0 = DirectX::XMFLOAT4( -1.0f, 0.0f, 0.0f, 0.0f );
	m_fParam1 = DirectX::XMFLOAT4( 1.0f, 0.1f, 0.0f, 0.0f );
}

CModel::~CModel()
{
	for( int ii = 0; ii < enTexType_Max; ii++ )
	{
		I_RELEASE( m_pShaderResourceView[ii] );
	}
	I_RELEASE( m_pVertexBuffer );
	I_RELEASE( m_pIndexBuffer );
	I_RELEASE( m_hpRasterizerState );
}

