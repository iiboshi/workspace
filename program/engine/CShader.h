/* Shader class */

#ifndef H_CSHADER
#define H_CSHADER

#include <vector>
#include <string>

class CShader
{
I_SINGLETON( CShader )
public:
	enum EnState
	{
		enState_Wrap,
		enState_Mirror,
		enState_Max,
	};
	enum EnRT
	{
		enRT_GBStart	= 0,	//< GBuffer.
		enRT_GBAlbed	= 0,
		enRT_GBNormal,
		enRT_GBDepth,
		enRT_GBParam,
		enRT_GBEnd		= 4,
		enRT_BlStart	= 4,	//< Blur.
		enRT_BlDest		= 4,
		enRT_BlSSSColor,
		enRT_BlSpec,
		enRT_BlSSS,
		enRT_BlEnd		= 6,
		enRT_Max		= 6,
	};
	struct StVertexShader
	{
		std::string name;
		ID3D11InputLayout*	pInputLayout;
		ID3D11VertexShader*	pVertexShader;
		StVertexShader()
		{
			pInputLayout	= nullptr;
			pVertexShader	= nullptr;
		}
	};
	struct StPixelShader
	{
		std::string name;
		ID3D11PixelShader*	pPixelShader;
		StPixelShader()
		{
			pPixelShader = nullptr;
		}
	};
	struct StRenderTarget
	{
		ID3D11Texture2D*			m_pTexture[enRT_Max];
		ID3D11RenderTargetView*		m_pRenderTargetView[enRT_Max];
		ID3D11ShaderResourceView*	m_pShaderResourceView[enRT_Max];
	};
	CShader();
	~CShader();
	void Release();
	HRESULT CreateSampler();
	HRESULT CreateRenderTarget();
	HRESULT CompileShaderFromFile(
		LPCSTR szFileName,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3DBlob** ppBlobOut );
	HRESULT CreateVertexShader( char* _filename, char* _filePath );
	HRESULT CreatePixelShader( char* _filename, char* _filePath );
	ID3D11InputLayout*	GetInputLayout( char* _filename );
	ID3D11VertexShader*	GetVertexShader( char* _filename );
	ID3D11PixelShader*	GetPixelShader( char* _filename );
	ID3D11SamplerState*	GetSamplerState( EnState _en );
public:
	std::vector<StVertexShader*>	m_listVertexShader;
	std::vector<StPixelShader*>		m_listPixelShader;
	ID3D11SamplerState*				m_pSamplerState[enState_Max];
	StRenderTarget					m_stRenderTarget;
};

#endif
