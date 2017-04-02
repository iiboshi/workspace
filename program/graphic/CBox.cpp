/* Box class */

#include "CBox.h"
#include "engine\CDevice.h"
#include "engine\DDSTextureLoader.h"

CBox::CBox()
{
	Create();
}

CBox::~CBox()
{
}

HRESULT CBox::Create()
{
	HRESULT hr;
	CDevice* pcDevice = CDevice::Instance();

	m_mWorldMatrix = DirectX::XMMatrixRotationX( I_PI );
	m_mWorldMatrix._41 -= 4.0f;
	m_mWorldMatrix._43 -= 4.0f;

	// Param
	m_fParam1.x = 0.6f;	//<! Roughness.

	// バッファーデスク
	D3D11_BUFFER_DESC		bd;
	D3D11_SUBRESOURCE_DATA	InitData;

	// バーテックスサイズを記憶
	m_uStride = sizeof( SimpleVertex );

	// バイトサイズ
	m_iIndexNum = 36;

	#define	TASU 0.5f
	#define	SIZE 1.0f
	SimpleVertex vertices[] =
	{
		// POSITION									// TEXCOORD							// NORMAL									// TANGENT								// COLOR
		{ DirectX::XMFLOAT3( -SIZE, SIZE, -SIZE ),	DirectX::XMFLOAT2( 1.0f, 0.0f ),	DirectX::XMFLOAT3( -TASU, 1.0f, -TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, SIZE, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3(  TASU, 1.0f, -TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, SIZE,  SIZE ),	DirectX::XMFLOAT2( 0.0f, 1.0f ),	DirectX::XMFLOAT3(  TASU, 1.0f,  TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE, SIZE,  SIZE ),	DirectX::XMFLOAT2( 1.0f, 1.0f ),	DirectX::XMFLOAT3( -TASU, 1.0f,  TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -SIZE, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( -TASU, -1.0f, -TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -SIZE, -SIZE ),	DirectX::XMFLOAT2( 1.0f, 0.0f ),	DirectX::XMFLOAT3(  TASU, -1.0f, -TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -SIZE,  SIZE ),	DirectX::XMFLOAT2( 1.0f, 1.0f ),	DirectX::XMFLOAT3(  TASU, -1.0f,  TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE, -SIZE,  SIZE ),	DirectX::XMFLOAT2( 0.0f, 1.0f ),	DirectX::XMFLOAT3( -TASU, -1.0f,  TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -SIZE,  SIZE ),	DirectX::XMFLOAT2( 1.0f, 0.0f ),	DirectX::XMFLOAT3( -1.0f, -TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE, -SIZE, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( -1.0f, -TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  SIZE, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 1.0f ),	DirectX::XMFLOAT3( -1.0f,  TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  SIZE,  SIZE ),	DirectX::XMFLOAT2( 1.0f, 1.0f ),	DirectX::XMFLOAT3( -1.0f,  TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( SIZE, -SIZE,  SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( 1.0f, -TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( SIZE, -SIZE, -SIZE ),	DirectX::XMFLOAT2( 1.0f, 0.0f ),	DirectX::XMFLOAT3( 1.0f, -TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( SIZE,  SIZE, -SIZE ),	DirectX::XMFLOAT2( 1.0f, 1.0f ),	DirectX::XMFLOAT3( 1.0f,  TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( SIZE,  SIZE,  SIZE ),	DirectX::XMFLOAT2( 0.0f, 1.0f ),	DirectX::XMFLOAT3( 1.0f,  TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -SIZE, -SIZE ),	DirectX::XMFLOAT2( 1.0f, 0.0f ),	DirectX::XMFLOAT3( -TASU, -TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -SIZE, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3(  TASU, -TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE,  SIZE, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 1.0f ),	DirectX::XMFLOAT3(  TASU,  TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  SIZE, -SIZE ),	DirectX::XMFLOAT2( 1.0f, 1.0f ),	DirectX::XMFLOAT3( -TASU,  TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -SIZE, SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( -TASU, -TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -SIZE, SIZE ),	DirectX::XMFLOAT2( 1.0f, 0.0f ),	DirectX::XMFLOAT3(  TASU, -TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE,  SIZE, SIZE ),	DirectX::XMFLOAT2( 1.0f, 1.0f ),	DirectX::XMFLOAT3(  TASU,  TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  SIZE, SIZE ),	DirectX::XMFLOAT2( 0.0f, 1.0f ),	DirectX::XMFLOAT3( -TASU,  TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) }
	};

	ZeroMemory( &bd, sizeof(bd) );
	ZeroMemory( &InitData, sizeof(InitData) );
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( SimpleVertex ) * 24;
	bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	InitData.pSysMem	= vertices;
	I_RETURN_M( pcDevice->m_pd3dDevice->CreateBuffer(
		&bd, &InitData, &m_pVertexBuffer ),
		"m_pVertexBuffer CreateBuffer に失敗" );

	// Create index buffer
	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( WORD ) * m_iIndexNum;
	bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	InitData.pSysMem	= indices;
	I_RETURN_M( pcDevice->m_pd3dDevice->CreateBuffer(
		&bd, &InitData, &m_pIndexBuffer ),
		"m_pIndexBuffer CreateBuffer に失敗" );

	// Load the Texture
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcTexPath[NDefine::enTexPath_wall], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Albedo], NULL ) );

	// Load the TextureNormal
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcNrmPath[NDefine::enNrmPath_wall_NM_height], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Normal], NULL ) );

	// Load the TextureMicrogeometry
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcNrmPath[NDefine::enNrmPath_normal], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Micro], NULL ) );

	return hr;
}
