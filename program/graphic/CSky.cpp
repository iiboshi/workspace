/* Sky class */

#include "CSky.h"
#include "engine\CDevice.h"
#include "engine\DDSTextureLoader.h"

CSky::CSky()
{
	Create();
}

CSky::~CSky()
{
}

HRESULT CSky::Create()
{
	HRESULT hr;
	CDevice* pcDevice = CDevice::Instance();

	m_mWorldMatrix = DirectX::XMMatrixRotationY( I_PI );
	m_mWorldMatrix._42 = 10.0f;

	// Param
	m_fParam0.w = 1.0f;	//!< w:Albedo Only.
	m_fParam1.x = 0.6f;	//<! Roughness.
	m_fParam2.x = 1.0f;	//<! Cube.

	// バッファーデスク
	D3D11_BUFFER_DESC		bd;
	D3D11_SUBRESOURCE_DATA	InitData;

	// バーテックスサイズを記憶
	m_uStride = sizeof( SimpleVertex );

	// バイトサイズ
	m_iIndexNum = 36;

	#define	TASU 1.0f
	#define	SIZE 50.0f
	#define	HEIG 20.0f
	#define TEXC 1.0f
	SimpleVertex vertices[] =
	{
		// POSITION									// TEXCOORD							// NORMAL									// TANGENT								// COLOR
		{ DirectX::XMFLOAT3( -SIZE, HEIG, -SIZE ),	DirectX::XMFLOAT2( TEXC, 0.0f ),	DirectX::XMFLOAT3( -TASU, 1.0f, -TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, HEIG, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3(  TASU, 1.0f, -TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, HEIG,  SIZE ),	DirectX::XMFLOAT2( 0.0f, TEXC ),	DirectX::XMFLOAT3(  TASU, 1.0f,  TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE, HEIG,  SIZE ),	DirectX::XMFLOAT2( TEXC, TEXC ),	DirectX::XMFLOAT3( -TASU, 1.0f,  TASU ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -HEIG, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( -TASU, -1.0f, -TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -HEIG, -SIZE ),	DirectX::XMFLOAT2( TEXC, 0.0f ),	DirectX::XMFLOAT3(  TASU, -1.0f, -TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -HEIG,  SIZE ),	DirectX::XMFLOAT2( TEXC, TEXC ),	DirectX::XMFLOAT3(  TASU, -1.0f,  TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE, -HEIG,  SIZE ),	DirectX::XMFLOAT2( 0.0f, TEXC ),	DirectX::XMFLOAT3( -TASU, -1.0f,  TASU ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -HEIG,  SIZE ),	DirectX::XMFLOAT2( TEXC, 0.0f ),	DirectX::XMFLOAT3( -1.0f, -TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE, -HEIG, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( -1.0f, -TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  HEIG, -SIZE ),	DirectX::XMFLOAT2( 0.0f, TEXC ),	DirectX::XMFLOAT3( -1.0f,  TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  HEIG,  SIZE ),	DirectX::XMFLOAT2( TEXC, TEXC ),	DirectX::XMFLOAT3( -1.0f,  TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, -1.0f ),	DirectX::XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( SIZE, -HEIG,  SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( 1.0f, -TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( SIZE, -HEIG, -SIZE ),	DirectX::XMFLOAT2( TEXC, 0.0f ),	DirectX::XMFLOAT3( 1.0f, -TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( SIZE,  HEIG, -SIZE ),	DirectX::XMFLOAT2( TEXC, TEXC ),	DirectX::XMFLOAT3( 1.0f,  TASU, -TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( SIZE,  HEIG,  SIZE ),	DirectX::XMFLOAT2( 0.0f, TEXC ),	DirectX::XMFLOAT3( 1.0f,  TASU,  TASU ),	DirectX::XMFLOAT3( 0.0f, 0.0f, 1.0f ),	DirectX::XMFLOAT4( 1.0f, 1.0f, 0.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -HEIG, -SIZE ),	DirectX::XMFLOAT2( TEXC, 0.0f ),	DirectX::XMFLOAT3( -TASU, -TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -HEIG, -SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3(  TASU, -TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE,  HEIG, -SIZE ),	DirectX::XMFLOAT2( 0.0f, TEXC ),	DirectX::XMFLOAT3(  TASU,  TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  HEIG, -SIZE ),	DirectX::XMFLOAT2( TEXC, TEXC ),	DirectX::XMFLOAT3( -TASU,  TASU, -1.0f ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 0.0f, 1.0f, 1.0f, 1.0f ) },

		{ DirectX::XMFLOAT3( -SIZE, -HEIG, SIZE ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3( -TASU, -TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE, -HEIG, SIZE ),	DirectX::XMFLOAT2( TEXC, 0.0f ),	DirectX::XMFLOAT3(  TASU, -TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  SIZE,  HEIG, SIZE ),	DirectX::XMFLOAT2( TEXC, TEXC ),	DirectX::XMFLOAT3(  TASU,  TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -SIZE,  HEIG, SIZE ),	DirectX::XMFLOAT2( 0.0f, TEXC ),	DirectX::XMFLOAT3( -TASU,  TASU, 1.0f ),	DirectX::XMFLOAT3( -1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 1.0f, 1.0f ) }
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
		0,1,3,
		3,1,2,

		5,4,6,
		6,4,7,

		8,9,11,
		11,9,10,

		13,12,14,
		14,12,15,

		16,17,19,
		19,17,18,

		21,20,22,
		22,20,23
	};

	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( WORD ) * m_iIndexNum;
	bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	InitData.pSysMem	= indices;
	I_RETURN_M( pcDevice->m_pd3dDevice->CreateBuffer(
		&bd, &InitData, &m_pIndexBuffer ),
		"m_pIndexBuffer CreateBuffer に失敗" );

	// Cube.
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, "../../Media/Textures/sky0.dds", 
		NULL, NULL, &m_pShaderResourceViewCube, NULL ) );

	// Load the Texture
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcTexPath[NDefine::enTexPath_white], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Albedo], NULL ) );

	// Load the TextureNormal
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcNrmPath[NDefine::enNrmPath_normal], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Normal], NULL ) );

	// Load the TextureMicrogeometry
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcNrmPath[NDefine::enNrmPath_normal], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Micro], NULL ) );

	return hr;
}
