/* Shader class */

#ifndef H_CSHADER
#define H_CSHADER

#include <vector>
#include <string>
#include <d3d11.h>

class CShader
{
I_SINGLETON( CShader )
public:
	enum EnState
	{
		enState_Wrap,
		enState_Mirror,
		enState_Clamp,
		enState_Max,
	};
	enum EnRT
	{
		enRT_GBStart		= 0,	//!< GBuffer.
		enRT_GBAlbed		= 0,
		enRT_GBNormal,
		enRT_GBDepth,
		enRT_GBParam,
		enRT_GBEnd,
		enRT_GBNum			= 4,
		enRT_SSStart		= 4,	//!< SSSS.
		enRT_SSDest			= 4,
		enRT_SSColor,
		enRT_SSSpec,
		enRT_SSParam,
		enRT_SSEnd,
		enRT_SSNum			= 4,
		enRT_ShadowStart	= 8,	//!< Shadow
		enRT_ShadowMap		= 8,
		enRT_Shadow,
		enRT_ShadowEnd,
		enRT_ShadowNum		= 2,
		enRT_Max			= 10,
	};
	enum EnWorkRT
	{
		enWorkRT0		= 0,
		enWorkRTmax,
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
		#if defined( USE_MSAA )
		ID3D11Texture2D*			m_pResolveTexture[enRT_Max];
		ID3D11ShaderResourceView*	m_pRenderTextureSRV[enRT_Max];
		DXGI_FORMAT					m_resolveFormat;
		#endif
	};
	CShader();
	~CShader();
	void Release();
	HRESULT CreateSampler();
	HRESULT CreateRenderTarget();
	HRESULT CompileShaderFromFile(
		LPCWSTR szFileName,
		LPCSTR szEntryPoint,
		LPCSTR szShaderModel,
		ID3DBlob** ppBlobOut );
	HRESULT CreateVertexShader( char* _filename, LPCWSTR _filePath );
	HRESULT CreatePixelShader( char* _filename, LPCWSTR _filePath );
	ID3D11InputLayout*	GetInputLayout( char* _filename );
	ID3D11VertexShader*	GetVertexShader( char* _filename );
	ID3D11PixelShader*	GetPixelShader( char* _filename );
	ID3D11SamplerState*	GetSamplerState( EnState _en );
public:
	std::vector<StVertexShader*>	m_listVertexShader;
	std::vector<StPixelShader*>		m_listPixelShader;
	ID3D11SamplerState*				m_pSamplerState[enState_Max];
	StRenderTarget					m_stRenderTarget;
	ID3D11Texture2D*				m_pWorkTexture[enWorkRTmax];
	ID3D11RenderTargetView*			m_pWorkRenderTargetView[enWorkRTmax];
	ID3D11ShaderResourceView*		m_pWorkShaderResourceView[enWorkRTmax];
};

#endif
