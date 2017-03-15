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
	int			m_iVertexNum;				//< �o�^���_�̐�.
	int			m_iSurfaceNum;				//< �ʂ̐�.
	int			m_iSurfaceIndexNum;			//< �ʂ̒��_��.
	int			m_iIndexNum;				//< �S�ʂ̒��_��.
	int			m_iMaterialNum;				//< �}�e���A����.
	int			m_iNormalNum;				//< �o�^�@���̐�.
	int			m_iNormalSurfaceNum;		//< �ʂ̐�.
	int			m_iNormalSurfaceIndexNum;	//< �ʂ̖@����.
	int			m_iNormalIndexNum;			//< �S�ʂ̖@����.
	int			m_iMeshTextureCoordsNum;	//< �o�^UV�̐�.
	int			m_iMeshVertexColorsNum;		//< �o�^���_�J���[�̐�.
	int			m_iTangentNum;				//< �ʂ̐�.
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
