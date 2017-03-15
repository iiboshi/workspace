/* CXFileLoder class */

#include "CXFileLoder.h"
#include <string>
#include <string.h>
#include <sstream>
#include <vector>

/*
xof 0302txt 0064　　　//可変長ヘッダーの定義

template Header {
<3D82AB43-62DA-11cf-AB39-0020AF71E433>
WORD major;
WORD minor;
DWORD flags;
}

//中略。　しばらく"template"の記述が続く

Mesh {　//頂点の定義。
	8;　//←頂点の数。　立方体の場合は8個と言う事。
	-1.00000;1.00000;-1.00000;,　//それぞれの頂点のｘ、ｙ、ｚ
	1.00000;1.00000;-1.00000;,
	1.00000;-1.00000;-1.00000;,
	-1.00000;-1.00000;-1.00000;,
	1.00000;1.00000;1.00000;,
	1.00000;-1.00000;1.00000;,
	-1.00000;1.00000;1.00000;,
	-1.00000;-1.00000;1.00000;;

	6;　//面の数。立方体（サイコロの形）の面の数は6個。
	4;0,1,2,3;,　//頂点インデックス。
	4;1,4,5,2;,　//面を作る頂点の数：　頂点の番号　という形式になっている。
	4;4,6,7,5;,　//立方体は四角形のポリゴンだけなので、すべて４つの頂点となっている。
	4;6,0,3,7;,
	4;6,4,1,0;,
	4;3,2,5,7;;

	MeshMaterialList {　//マテリアルのリスト。表面の色や、テクスチャについての情報。
		1;　//マテリアルの数。
		6;　//面の数。
		0,　//それぞれの面に、何番のマテリアルを適用するか？の指定。
		0,　//今回はマテリアルの数は1個なので、全部マテリアル番号0番となっている。
		0,
		0,
		0,
		0;;
		Material {　//マテリアルの定義。
			1.000000;1.000000;1.000000;1.000000;;　//赤、緑、青、透明度の指定
			0.000000;　//"鏡面反射"の強さ（power）
			0.000000;0.000000;0.000000;;　//"鏡面反射光"の成分（Specular）
			0.000000;0.000000;0.000000;;　//"発散光"の成分（Emissive）
			TextureFilename {
				"Weapons.dds";
			}

			EffectInstance {
				"reflect.fx";
				EffectParamString {
					"g_txScene";
					"Weapons.dds";
				}
				EffectParamString {
					"g_txEnvMap";
					"Lobby\\LobbyCube.dds";
				}
				EffectParamFloats {
					"Diffuse";
					4;
					1.000000,1.000000,1.000000,1.000000;
				}
				EffectParamFloats {
					"Specular";
					4;
					1.000000,1.000000,1.000000,1.000000;
				}
				EffectParamFloats {
					"Reflectivity";
					1;
					0.800000;
				}
				EffectParamFloats {
					"Power";
					1;
					20.000000;
				}
			}
		}

		Material {
			0.600000;0.600000;1.000000;1.000000;;
			40.000000;
			1.000000;1.000000;1.000000;;
			0.000000;0.000000;0.000000;;

			TextureFilename {
				"cellwall.jpg";
			}
		}

		Material {
			1.000000;1.000000;1.000000;1.000000;;
			40.000000;
			1.000000;1.000000;1.000000;;
			0.000000;0.000000;0.000000;;

			TextureFilename {
				"cellceiling.jpg";
			}
		}
	}
	MeshNormals {　//法線の定義 （面に当たった光がどの方向へ反射するか？）
		6;　//法線の数。　今回は6個。
		0.000000;0.000000;-1.000000;,　//法線のx,y,z
		1.000000;0.000000;0.000000;,
		0.000000;0.000000;1.000000;,
		-1.000000;0.000000;0.000000;,
		0.000000;1.000000;0.000000;,
		0.000000;-1.000000;-0.000000;;
		6;　//面の数。　立方体の面は6個。
		4;0,0,0,0;,　//面番号'0'に、法線番号'0'を指定する。
		4;1,1,1,1;,　（面番号は、上の方で定義してある。↑）
		4;2,2,2,2;,
		4;3,3,3,3;,
		4;4,4,4,4;,
		4;5,5,5,5;;
	}
}
*/

namespace
{
	// Split
	std::vector<std::string> split( const std::string &str, char sep )
	{
		std::vector<std::string> v;
		std::stringstream ss( str );
		std::string buffer;
		while( std::getline( ss, buffer, sep ) )
		{
			v.push_back( buffer );
		}
		return v;
	}

	void Vec3Normalize( DirectX::XMFLOAT3* out, DirectX::XMFLOAT3* in )
	{
		float nrm = pow( 
			( in->x * in->x ) + 
			( in->y * in->y ) + 
			( in->z * in->z ), 0.5f );
		out->x = in->x / nrm;
		out->y = in->y / nrm;
		out->z = in->z / nrm;
	}

	void Vec3Cross( DirectX::XMFLOAT3* out, DirectX::XMFLOAT3* v0, DirectX::XMFLOAT3* v1 )
	{
		out->x = ( v0->y * v1->z ) - ( v0->z * v1->y );
		out->y = ( v0->z * v1->x ) - ( v0->x * v1->z );
		out->z = ( v0->x * v1->y ) - ( v0->y * v1->x );
	}

	void CalcTangent(
		DirectX::XMFLOAT3* p0,	DirectX::XMFLOAT3* p1,	DirectX::XMFLOAT3* p2,
		DirectX::XMFLOAT2* uv0,	DirectX::XMFLOAT2* uv1,	DirectX::XMFLOAT2* uv2,
		DirectX::XMFLOAT3* outTangent )
	{
		DirectX::XMFLOAT3 CP0[ 3 ] = {
			DirectX::XMFLOAT3( p0->x, uv0->x, uv0->y ),
			DirectX::XMFLOAT3( p0->y, uv0->x, uv0->y ),
			DirectX::XMFLOAT3( p0->z, uv0->x, uv0->y ),
		};
		DirectX::XMFLOAT3 CP1[ 3 ] = {
			DirectX::XMFLOAT3( p1->x, uv1->x, uv1->y ),
			DirectX::XMFLOAT3( p1->y, uv1->x, uv1->y ),
			DirectX::XMFLOAT3( p1->z, uv1->x, uv1->y ),
		};
		DirectX::XMFLOAT3 CP2[ 3 ] = {
			DirectX::XMFLOAT3( p2->x, uv2->x, uv2->y ),
			DirectX::XMFLOAT3( p2->y, uv2->x, uv2->y ),
			DirectX::XMFLOAT3( p2->z, uv2->x, uv2->y ),
		};

		float ret[3];
		for ( int ii = 0; ii < 3; ii++ )
		{
			DirectX::XMFLOAT3 V1;
			V1.x = CP1[ii].x - CP0[ii].x;
			V1.y = CP1[ii].y - CP0[ii].y;
			V1.z = CP1[ii].z - CP0[ii].z;
			DirectX::XMFLOAT3 V2;
			V2.x = CP2[ii].x - CP1[ii].x;
			V2.y = CP2[ii].y - CP1[ii].y;
			V2.z = CP2[ii].z - CP1[ii].z;
			DirectX::XMFLOAT3 work;
			Vec3Cross( &work, &V1, &V2 );

			if ( work.x == 0.0f )
			{
				_ASSERT( 0 );
				memset( outTangent, 0, sizeof( DirectX::XMFLOAT3 ) );
				return;
			}
			ret[ii] = - work.y / work.x;
		}
		memcpy( outTangent, ret, sizeof( float ) * 3 );
		Vec3Normalize( outTangent, outTangent );
	}
}

CXFileLoder::CXFileLoder( char* _path  )
	: m_iVertexNum				( 0 )
	, m_iSurfaceNum				( 0 )
	, m_iSurfaceIndexNum		( 0 )
	, m_iIndexNum				( 0 )
	, m_iMaterialNum			( 0 )
	, m_iNormalNum				( 0 )
	, m_iNormalSurfaceNum		( 0 )
	, m_iNormalSurfaceIndexNum	( 0 )
	, m_iNormalIndexNum			( 0 )
	, m_iMeshTextureCoordsNum	( 0 )
	, m_iMeshVertexColorsNum	( 0 )
	, m_iTangentNum				( 0 )
	, m_pVertex					( nullptr )
	, m_pIndex					( nullptr )
	, m_pMaterialList			( nullptr )
	, m_pMaterial				( nullptr )
	, m_pNormal					( nullptr )
	, m_pNormalIndex			( nullptr )
	, m_pMeshTextureCoords		( nullptr )
	, m_pMeshVertexColors		( nullptr )
	, m_pTangent				( nullptr )
{
	Load( _path );
}

CXFileLoder::~CXFileLoder()
{
	I_DELETE_ARRAY( m_pVertex );
	I_DELETE_ARRAY( m_pIndex );
	I_DELETE_ARRAY( m_pMaterialList );
	I_DELETE_ARRAY( m_pMaterial );
	I_DELETE_ARRAY( m_pNormal );
	I_DELETE_ARRAY( m_pNormalIndex );
	I_DELETE_ARRAY( m_pMeshTextureCoords );
	I_DELETE_ARRAY( m_pMeshVertexColors );
	I_DELETE_ARRAY( m_pTangent );
}

bool CXFileLoder::Load( const char *_path )
{
	FILE* pFile = nullptr;

	fopen_s( &pFile, _path, "rb" );
	if( pFile == nullptr )
	{
		return false;
	}

	bool bQuad = false;
	const int iCnt = 128;
	char cBuff[iCnt];
	memset( cBuff, 0, sizeof( char ) * iCnt );

	// Mesh
	{
		int ii = 0;
		bool bMesh = false;
		while ( fgets( cBuff, iCnt, pFile ) != nullptr )
		{
			if( !bMesh )
			{
				if( strstr( cBuff, "template" ) == NULL && strstr( cBuff, "Mesh " ) != NULL )
				{
					fgets( cBuff, iCnt, pFile );
					m_iVertexNum = std::atoi( cBuff );
					m_pVertex = I_NEW( DirectX::XMFLOAT3[m_iVertexNum] );
					memset( m_pVertex, 0, sizeof( DirectX::XMFLOAT3 ) * m_iVertexNum );
					bMesh = true;
				}
			}
			else
			{
				std::vector<std::string> list = split( cBuff, ';' );
				auto str = list.begin();
				m_pVertex[ii].x = (float)atof( ( str++ )->data() );
				m_pVertex[ii].y = (float)atof( ( str++ )->data() );
				m_pVertex[ii].z = (float)atof( ( str )->data() );
				ii++;
				if( strstr( cBuff, ";;" ) )
				{
					break;
				}
			}
		}
	}

	// Index
	{
		int ii = 0;
		bool bIndex = false;
		while ( fgets( cBuff, iCnt, pFile ) != nullptr )
		{
			if( !bIndex )
			{
				if( strstr( cBuff, ";" ) != NULL )
				{
					m_iSurfaceNum = std::atoi( cBuff );
					bIndex = true;
				}
			}
			else
			{
				if( m_pIndex == nullptr )
				{
					m_iSurfaceIndexNum = atoi( cBuff );
					bQuad = ( m_iSurfaceIndexNum == 4 );
					if( bQuad )
					{
						bQuad = true;
						m_iSurfaceNum *= 2;
						m_iIndexNum = 3 * m_iSurfaceNum;
						m_pIndex = I_NEW( WORD[m_iIndexNum] );
						memset( m_pIndex, 0, sizeof( WORD ) * m_iIndexNum );
					}
					else
					{
						m_iIndexNum = m_iSurfaceIndexNum * m_iSurfaceNum;
						m_pIndex = I_NEW( WORD[m_iIndexNum] );
						memset( m_pIndex, 0, sizeof( WORD ) * m_iIndexNum );
					}
				}
				std::vector<std::string> list0 = split( cBuff, ';' );
				auto str0 = list0.begin();
				str0++;
				std::vector<std::string> list1 = split( str0->data(), ',' );
				auto str1 = list1.begin();
				if( bQuad )
				{
					WORD work[4];
					for( int cnt = 0; cnt < m_iSurfaceIndexNum; cnt++ )
					{
						work[cnt] = (WORD)atoi( ( str1++ )->data() );
					}
					m_pIndex[ii++] = work[0];
					m_pIndex[ii++] = work[1];
					m_pIndex[ii++] = work[3];

					m_pIndex[ii++] = work[1];
					m_pIndex[ii++] = work[2];
					m_pIndex[ii++] = work[3];
				}
				else
				{
					for( int cnt = 0; cnt < m_iSurfaceIndexNum; cnt++ )
					{
						m_pIndex[ii++] = (WORD)atoi( ( str1++ )->data() );
					}
				}
				if( strstr( cBuff, ";;" ) )
				{
					break;
				}
			}
		}
		if( bQuad )
		{
			m_iSurfaceIndexNum = 3;
		}
	}

	// MeshMaterialList
	{
		int ii = 0;
		bool bMeshMaterialList = false;
		while ( fgets( cBuff, iCnt, pFile ) != nullptr )
		{
			if( !bMeshMaterialList )
			{
				if( strstr( cBuff, "MeshMaterialList" ) != NULL )
				{
					fgets( cBuff, iCnt, pFile );
					m_iMaterialNum = (int)atoi( cBuff );
					fgets( cBuff, iCnt, pFile );
					m_pMaterialList = I_NEW( int[m_iSurfaceNum] );
					memset( m_pMaterialList, 0, sizeof( int ) * m_iSurfaceNum );
					bMeshMaterialList = true;
				}
			}
			else
			{
				m_pMaterialList[ii++] = (int)atoi( cBuff );
				if( bQuad )
				{
					m_pMaterialList[ii++] = m_pMaterialList[ii];
				}
				if( strstr( cBuff, ";;" ) )
				{
					break;
				}
			}
		}
	}

	// Material
	{
		int ii = 0;
		bool bMaterial = false;
		while ( fgets( cBuff, iCnt, pFile ) != nullptr )
		{
			if( !bMaterial )
			{
				if( strstr( cBuff, "Material" ) != NULL )
				{
					m_pMaterial = I_NEW( StMaterial[m_iMaterialNum] );
					memset( m_pMaterial, 0, sizeof( StMaterial ) * m_iMaterialNum );
					bMaterial = true;
				}
			}
			else
			{
				// Color.
				{
					std::vector<std::string> list = split( cBuff, ';' );
					auto str = list.begin();
					m_pMaterial[ii].m_vColor.x = (float)atof( ( str++ )->data() );
					m_pMaterial[ii].m_vColor.y = (float)atof( ( str++ )->data() );
					m_pMaterial[ii].m_vColor.z = (float)atof( ( str++ )->data() );
					m_pMaterial[ii].m_vColor.w = (float)atof( ( str )->data() );
				}
				// Power.
				{
					fgets( cBuff, iCnt, pFile );
					m_pMaterial[ii].m_fPower = (float)atof( cBuff );
				}
				// Specular
				{
					fgets( cBuff, iCnt, pFile );
					std::vector<std::string> list = split( cBuff, ';' );
					auto str = list.begin();
					m_pMaterial[ii].m_fSpecular.x = (float)atof( ( str++ )->data() );
					m_pMaterial[ii].m_fSpecular.y = (float)atof( ( str++ )->data() );
					m_pMaterial[ii].m_fSpecular.z = (float)atof( ( str )->data() );
				}
				// Emissive.
				{
					fgets( cBuff, iCnt, pFile );
					std::vector<std::string> list = split( cBuff, ';' );
					auto str = list.begin();
					m_pMaterial[ii].m_fEmissive.x = (float)atof( ( str++ )->data() );
					m_pMaterial[ii].m_fEmissive.y = (float)atof( ( str++ )->data() );
					m_pMaterial[ii].m_fEmissive.z = (float)atof( ( str )->data() );
				}
				// TextureFilename
				{
					fgets( cBuff, iCnt, pFile );
					if( strstr( cBuff, "TextureFilename" ) != NULL )
					{
						fgets( cBuff, iCnt, pFile );
						std::string str = cBuff;
						int del = (int)str.rfind(";");
						str.replace( del, 1, "\0" );
						if( str.length() < STRNUM )
						{
							memcpy( m_pMaterial[ii].m_cTexName, str.data(), sizeof( char ) * str.length() );
						}
					}
				}
				bMaterial = false;
				ii++;
				if( ii == m_iMaterialNum )
				{
					break;
				}
			}
		}
	}

	// MeshNormals
	{
		int ii = 0;
		bool bMeshNormals = false;
		while ( fgets( cBuff, iCnt, pFile ) != nullptr )
		{
			if( !bMeshNormals )
			{
				if( strstr( cBuff, "MeshNormals" ) != NULL )
				{
					fgets( cBuff, iCnt, pFile );
					m_iNormalNum = std::atoi( cBuff );
					m_pNormal = I_NEW( DirectX::XMFLOAT3[m_iNormalNum] );
					memset( m_pNormal, 0, sizeof( DirectX::XMFLOAT3 ) * m_iNormalNum );
					bMeshNormals = true;
				}
			}
			else
			{
				std::vector<std::string> list = split( cBuff, ';' );
				auto str = list.begin();
				m_pNormal[ii].x = (float)atof( ( str++ )->data() );
				m_pNormal[ii].y = (float)atof( ( str++ )->data() );
				m_pNormal[ii].z = (float)atof( ( str )->data() );
				ii++;
				if( strstr( cBuff, ";;" ) )
				{
					break;
				}
			}
		}
	}

	// MeshNormals Index
	{
		int ii = 0;
		bool bMeshNormals = false;
		while ( fgets( cBuff, iCnt, pFile ) != nullptr )
		{
			if( !bMeshNormals )
			{
				if( strstr( cBuff, ";" ) != NULL )
				{
					m_iNormalSurfaceNum = std::atoi( cBuff );
					bMeshNormals = true;
				}
			}
			else
			{
				if( m_pNormalIndex == nullptr )
				{
					m_iNormalSurfaceIndexNum = atoi( cBuff );
					if( bQuad )
					{
						m_iNormalSurfaceNum *= 2;
						m_iNormalIndexNum = 3 * m_iNormalSurfaceNum;
						m_pNormalIndex = I_NEW( WORD[m_iNormalIndexNum] );
						memset( m_pNormalIndex, 0, sizeof( WORD ) * m_iNormalIndexNum );
					}
					else
					{
						m_iNormalIndexNum = m_iNormalSurfaceIndexNum * m_iNormalSurfaceNum;
						m_pNormalIndex = I_NEW( WORD[m_iNormalIndexNum] );
						memset( m_pNormalIndex, 0, sizeof( WORD ) * m_iNormalIndexNum );
					}
				}
				std::vector<std::string> list0 = split( cBuff, ';' );
				auto str0 = list0.begin();
				str0++;
				std::vector<std::string> list1 = split( str0->data(), ',' );
				auto str1 = list1.begin();
				if( bQuad )
				{
					WORD work[4];
					for( int cnt = 0; cnt < m_iNormalSurfaceIndexNum; cnt++ )
					{
						work[cnt] = (WORD)atoi( ( str1++ )->data() );
					}
					m_pNormalIndex[ii++] = work[0];
					m_pNormalIndex[ii++] = work[1];
					m_pNormalIndex[ii++] = work[3];

					m_pNormalIndex[ii++] = work[1];
					m_pNormalIndex[ii++] = work[2];
					m_pNormalIndex[ii++] = work[3];
				}
				else
				{
					for( int cnt = 0; cnt < m_iNormalSurfaceIndexNum; cnt++ )
					{
						m_pNormalIndex[ii++] = (WORD)atoi( ( str1++ )->data() );
					}
				}
				if( strstr( cBuff, ";;" ) )
				{
					break;
				}
			}
		}
		if( bQuad )
		{
			m_iNormalSurfaceIndexNum = 3;
		}
	}

	// MeshTextureCoords
	{
		int ii = 0;
		bool bMeshTextureCoords = false;
		while ( fgets( cBuff, 256, pFile ) != nullptr )
		{
			if( !bMeshTextureCoords )
			{
				if( strstr( cBuff, "MeshTextureCoords" ) != NULL )
				{
					fgets( cBuff, 256, pFile );
					m_iMeshTextureCoordsNum = atoi( cBuff );
					m_pMeshTextureCoords = I_NEW( DirectX::XMFLOAT2[m_iMeshTextureCoordsNum] );
					memset( m_pMeshTextureCoords, 0, sizeof( DirectX::XMFLOAT2 ) * m_iMeshTextureCoordsNum );
					bMeshTextureCoords = true;
				}
			}
			else
			{
				std::vector<std::string> list = split( cBuff, ';' );
				auto str = list.begin();
				m_pMeshTextureCoords[ii].x = (float)atof( ( str++ )->data() );
				m_pMeshTextureCoords[ii].y = (float)atof( ( str )->data() );
				ii++;
				if( strstr( cBuff, ";;" ) )
				{
					break;
				}
			}
		}
	}

	// MeshVertexColors
	{
		int ii = 0;
		bool bMeshVertexColors = false;
		while ( fgets( cBuff, 256, pFile ) != nullptr )
		{
			if( !bMeshVertexColors )
			{
				if( strstr( cBuff, "MeshVertexColors" ) != NULL )
				{
					fgets( cBuff, 256, pFile );
					m_iMeshVertexColorsNum = atoi( cBuff );
					m_pMeshVertexColors = I_NEW( DirectX::XMFLOAT4[m_iMeshVertexColorsNum] );
					memset( m_pMeshVertexColors, 0, sizeof( DirectX::XMFLOAT2 ) * m_iMeshVertexColorsNum );
					bMeshVertexColors = true;
				}
			}
			else
			{
				std::vector<std::string> list = split( cBuff, ';' );
				auto str = list.begin();
				str++;
				m_pMeshVertexColors[ii].x = (float)atof( ( str++ )->data() );
				m_pMeshVertexColors[ii].y = (float)atof( ( str++ )->data() );
				m_pMeshVertexColors[ii].z = (float)atof( ( str++ )->data() );
				m_pMeshVertexColors[ii].w = (float)atof( ( str )->data() );
				ii++;
				if( strstr( cBuff, ";;" ) )
				{
					break;
				}
			}
		}
	}

	fclose( pFile );

	// Tangent
	if( m_pTangent == nullptr )
	{
		int indexCnt = 0;
		m_iTangentNum = m_iVertexNum;
		m_pTangent = I_NEW( DirectX::XMFLOAT3[m_iVertexNum] );
		for( int ii = 0; ii < m_iSurfaceNum; ii++ )
		{
			// Find
			int index[4];
			for( int jj = 0; jj < m_iSurfaceIndexNum; jj++ )
			{
				index[jj] = m_pIndex[indexCnt];
				indexCnt++;
			}
			// Set
			{
				// Assert 回避
				m_pMeshTextureCoords[index[0]].x += 0.0000f;
				m_pMeshTextureCoords[index[1]].x += 0.0001f;
				m_pMeshTextureCoords[index[2]].x += 0.0002f;
				m_pMeshTextureCoords[index[0]].y += 0.0003f;
				m_pMeshTextureCoords[index[1]].y += 0.0004f;
				m_pMeshTextureCoords[index[2]].y += 0.0005f;

				DirectX::XMFLOAT3 f3Cal;
				CalcTangent(
					&m_pVertex[index[0]], 
					&m_pVertex[index[1]], 
					&m_pVertex[index[2]],
					&m_pMeshTextureCoords[index[0]], 
					&m_pMeshTextureCoords[index[1]], 
					&m_pMeshTextureCoords[index[2]],
					&f3Cal );
				for( int jj = 0; jj < m_iSurfaceIndexNum; jj++ )
				{
					m_pTangent[index[jj]] = f3Cal;
				}
			}
		}
	}

	return true;
}
