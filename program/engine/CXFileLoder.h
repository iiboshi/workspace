/* ModelLoder class */

#ifndef H_CXFILELODER
#define H_CXFILELODER

#include <d3d11.h>
#include <DirectXMath.h>

class CXFileLoder
{
public:
	struct StMaterial
	{
		DirectX::XMFLOAT4	m_vColor;
		float		m_fPower;
		DirectX::XMFLOAT3	m_fSpecular;
		DirectX::XMFLOAT3	m_fEmissive;
		char		m_cTexName[STRNUM];
	};
	CXFileLoder( char* _path );
	~CXFileLoder();
	bool Load( const char *filename );
public:
	int			m_iVertexNum;				//!< 登録頂点の数.
	int			m_iSurfaceNum;				//!< 面の数.
	int			m_iSurfaceIndexNum;			//!< 面の頂点数.
	int			m_iIndexNum;				//!< 全面の頂点数.
	int			m_iMaterialNum;				//!< マテリアル数.
	int			m_iNormalNum;				//!< 登録法線の数.
	int			m_iNormalSurfaceNum;		//!< 面の数.
	int			m_iNormalSurfaceIndexNum;	//!< 面の法線数.
	int			m_iNormalIndexNum;			//!< 全面の法線数.
	int			m_iMeshTextureCoordsNum;	//!< 登録UVの数.
	int			m_iMeshVertexColorsNum;		//!< 登録頂点カラーの数.
	int			m_iTangentNum;				//!< 面の数.
	DirectX::XMFLOAT3*	m_pVertex;
	WORD*		m_pIndex;
	int*		m_pMaterialList;
	StMaterial*	m_pMaterial;
	DirectX::XMFLOAT3*	m_pNormal;
	WORD*		m_pNormalIndex;
	DirectX::XMFLOAT2*	m_pMeshTextureCoords;
	DirectX::XMFLOAT4*	m_pMeshVertexColors;
	DirectX::XMFLOAT3*	m_pTangent;
};

#endif
