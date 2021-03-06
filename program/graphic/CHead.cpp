/* CHead class */

#include "CHead.h"
#include "engine\CDevice.h"
#include "engine\CXFileLoder.h"
#include "engine\DDSTextureLoader.h"
#include <process.h>

HANDLE CHead::m_hThread;

CHead::CHead()
{
	Create();
}

CHead::~CHead()
{
}

void CHead::Create()
{
	m_bDraw = false;

	m_hThread = ( HANDLE )_beginthreadex( 
		NULL, 
		0, 
		CHead::Load, 
		this, 
		CREATE_SUSPENDED, 
		NULL );
	ResumeThread( m_hThread );
}

unsigned int WINAPI CHead::Load( LPVOID lpParameter )
{
	CHead* pcHead = static_cast<CHead*>( lpParameter );

	HRESULT hr = S_OK;;
	CDevice* pcDevice = CDevice::Instance();

	// Matrix
	DirectX::XMMATRIX mMatrixScale = DirectX::XMMatrixScaling( 0.009f, 0.009f, 0.009f );
	DirectX::XMMATRIX mMatrixRot = DirectX::XMMatrixRotationY( I_PI );
	pcHead->m_mWorldMatrix *= mMatrixScale;
	pcHead->m_mWorldMatrix *= mMatrixRot;
	pcHead->m_mWorldMatrix._42 += 1.0f;
	pcHead->m_mWorldMatrix._43 += 1.0f;

	// Param
	pcHead->m_fParam0.x = 1.0f;		//<! x:Normal X 補正.
	pcHead->m_fParam0.y = 15.0f;	//<! Detail 回数.
	pcHead->m_fParam0.z = 2.0f;		//<! Detail 強度.
	pcHead->m_fParam1.x = 0.8f;		//<! Roughness.
	pcHead->m_fParam1.y = 0.028f;	//<! Fresnel.
	pcHead->m_fParam1.z = 1.0f;		//<! SSS.
	pcHead->m_fParam2.y = 1.0f;		//<! Microgeometry Mask.

	// バッファーデスク
	D3D11_BUFFER_DESC		bd;
	D3D11_SUBRESOURCE_DATA	InitData;

	// バーテックスサイズを記憶
	pcHead->m_uStride = sizeof( SimpleVertex );

	CXFileLoder cXFileLoder( "../../model/head/head.x" );

	pcHead->m_iIndexNum = cXFileLoder.m_iIndexNum;

	SimpleVertex* pSV = I_NEW( SimpleVertex[cXFileLoder.m_iVertexNum] );
	for( int ii = 0; ii < cXFileLoder.m_iVertexNum; ii++ )
	{
		pSV[ii].Pos = cXFileLoder.m_pVertex[ii];
		pSV[ii].Tex = cXFileLoder.m_pMeshTextureCoords[ii];
		pSV[ii].Nrm = cXFileLoder.m_pNormal[cXFileLoder.m_pNormalIndex[ii]];
		pSV[ii].Tan = cXFileLoder.m_pTangent[ii];
		pSV[ii].Def = cXFileLoder.m_pMeshVertexColors[ii];
	}
	for( int ii = 0; ii < cXFileLoder.m_iIndexNum; ii++ )
	{
		pSV[cXFileLoder.m_pIndex[ii]].Nrm = cXFileLoder.m_pNormal[cXFileLoder.m_pNormalIndex[ii]];
	}

	ZeroMemory( &bd, sizeof(bd) );
	ZeroMemory( &InitData, sizeof(InitData) );
	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( SimpleVertex ) * cXFileLoder.m_iVertexNum;
	bd.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	InitData.pSysMem	= pSV;
	I_RETURN_M( pcDevice->m_pd3dDevice->CreateBuffer(
		&bd, &InitData, &pcHead->m_pVertexBuffer ),
		"m_pVertexBuffer CreateBuffer に失敗" );

	bd.Usage			= D3D11_USAGE_DEFAULT;
	bd.ByteWidth		= sizeof( WORD ) * pcHead->m_iIndexNum;
	bd.BindFlags		= D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags	= 0;
	InitData.pSysMem	= cXFileLoder.m_pIndex;
	I_RETURN_M( pcDevice->m_pd3dDevice->CreateBuffer(
		&bd, &InitData, &pcHead->m_pIndexBuffer ),
		"m_pIndexBuffer CreateBuffer に失敗" );

	// Load the Texture
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, "../../model/head/head.dds", 
		NULL, NULL, &pcHead->m_pShaderResourceView[enTexType_Albedo], NULL ) );

	// Load the TextureNormal
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, "../../model/head/head_n.dds", 
		NULL, NULL, &pcHead->m_pShaderResourceView[enTexType_Normal], NULL ) );

	// Load the TextureMicrogeometry
	I_RETURN( D3DX11CreateShaderResourceViewFromFile( 
		pcDevice->m_pd3dDevice, "../../model/head/microgeometry.dds", 
		NULL, NULL, &pcHead->m_pShaderResourceView[enTexType_Micro], NULL ) );

	pcHead->m_bDraw = true;

	return hr;
}
