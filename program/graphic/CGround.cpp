/* Ground class */

#include "CGround.h"
#include "engine\CDevice.h"
#include "engine\DDSTextureLoader.h"

CGround::CGround()
{
	Create();
}

CGround::~CGround()
{
}

HRESULT CGround::Create()
{
	HRESULT hr;
	CDevice* pcDevice = CDevice::Instance();

	// Param
	m_fParam1.x = 0.8f;
	m_fParam1.y = 0.02f;

	// バッファーデスク
	D3D11_BUFFER_DESC		bd;
	D3D11_SUBRESOURCE_DATA	InitData;

	// バーテックスサイズを記憶
	m_uStride = sizeof( SimpleVertex );

	// バイトサイズ
	m_iIndexNum = 6;

	#define TNUM	10.0f
	#define UNUM	TNUM / 2.0f
	#define KATA	0.0f
	SimpleVertex vertices[] =
	{
		// POSITION									// TEXCOORD							// NORMAL									// TANGENT								// COLOR
		{ DirectX::XMFLOAT3( -TNUM, -1.8f, -TNUM ),	DirectX::XMFLOAT2( UNUM, 0.0f ),	DirectX::XMFLOAT3( -KATA, 1.0f, -KATA ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  TNUM, -1.8f, -TNUM ),	DirectX::XMFLOAT2( 0.0f, 0.0f ),	DirectX::XMFLOAT3(  KATA, 1.0f, -KATA ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3(  TNUM, -1.8f,  TNUM ),	DirectX::XMFLOAT2( 0.0f, UNUM ),	DirectX::XMFLOAT3(  KATA, 1.0f,  KATA ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
		{ DirectX::XMFLOAT3( -TNUM, -1.8f,  TNUM ),	DirectX::XMFLOAT2( UNUM, UNUM ),	DirectX::XMFLOAT3( -KATA, 1.0f,  KATA ),	DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f ),	DirectX::XMFLOAT4( 1.0f, 0.0f, 0.0f, 1.0f ) },
	};

	ZeroMemory( &bd, sizeof(bd) );
	ZeroMemory( &InitData, sizeof(InitData) );
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( SimpleVertex ) * 4;
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
		pcDevice->m_pd3dDevice, NDefine::g_pcTexPath[NDefine::enTexPath_stones], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Albedo], NULL ) );

	// Load the TextureNormal
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcNrmPath[NDefine::enNrmPath_stones_NM_height], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Normal], NULL ) );

	// Load the TextureMicrogeometry
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, NDefine::g_pcNrmPath[NDefine::enNrmPath_normal], 
		NULL, NULL, &m_pShaderResourceView[enTexType_Micro], NULL ) );

	return hr;
}
