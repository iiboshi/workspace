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
	int			m_iVertexNum;				//< “o˜^’¸“_‚Ì”.
	int			m_iSurfaceNum;				//< –Ê‚Ì”.
	int			m_iSurfaceIndexNum;			//< –Ê‚Ì’¸“_”.
	int			m_iIndexNum;				//< ‘S–Ê‚Ì’¸“_”.
	int			m_iMaterialNum;				//< ƒ}ƒeƒŠƒAƒ‹”.
	int			m_iNormalNum;				//< “o˜^–@ü‚Ì”.
	int			m_iNormalSurfaceNum;		//< –Ê‚Ì”.
	int			m_iNormalSurfaceIndexNum;	//< –Ê‚Ì–@ü”.
	int			m_iNormalIndexNum;			//< ‘S–Ê‚Ì–@ü”.
	int			m_iMeshTextureCoordsNum;	//< “o˜^UV‚Ì”.
	int			m_iMeshVertexColorsNum;		//< “o˜^’¸“_ƒJƒ‰[‚Ì”.
	int			m_iTangentNum;				//< –Ê‚Ì”.
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
