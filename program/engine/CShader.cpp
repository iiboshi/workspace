/* Shader class */

#include <windows.h>
#include <tchar.h>
#include <vector>
#include <d3dcompiler.h>
#include <D3Dcommon.h>
#include "CShader.h"
#include "CDevice.h"

CShader::CShader()
{
	for( int ii = 0; ii < enState_Max; ii++ )
	{
		m_pSamplerState[ii] = nullptr;
	}
	for( int ii = 0; ii < enRT_Max; ii++ )
	{
		m_stRenderTarget.m_pTexture[ii]				= nullptr;
		m_stRenderTarget.m_pRenderTargetView[ii]	= nullptr;
		m_stRenderTarget.m_pShaderResourceView[ii]	= nullptr;
		#if defined( USE_MSAA )
		m_stRenderTarget.m_pResolveTexture[ii]		= nullptr;
		m_stRenderTarget.m_pRenderTextureSRV[ii]	= nullptr;
		#endif
	}
	for( int ii = 0; ii < enWorkRTmax; ii++ )
	{
		m_pWorkTexture[ii]				= nullptr;
		m_pWorkRenderTargetView[ii]		= nullptr;
		m_pWorkShaderResourceView[ii]	= nullptr;
	}
	CreateSampler();
	CreateRenderTarget();
}

CShader::~CShader()
{
	Release();
}

void CShader::Release()
{
	for( auto obj = m_listVertexShader.begin(); obj != m_listVertexShader.end(); obj++ )
	{
		if( *obj != nullptr )
		{
			I_RELEASE( (*obj)->pInputLayout );
			I_RELEASE( (*obj)->pVertexShader );
		}
	}
	for( auto obj = m_listPixelShader.begin(); obj != m_listPixelShader.end(); obj++ )
	{
		if( *obj != nullptr )
		{
			I_RELEASE( (*obj)->pPixelShader );
		}
	}
	for( int ii = 0; ii < enState_Max; ii++ )
	{
		I_RELEASE( m_pSamplerState[ii] );
	}
	for( int ii = 0; ii < enRT_Max; ii++ )
	{
		I_RELEASE( m_stRenderTarget.m_pTexture[ii] );
		I_RELEASE( m_stRenderTarget.m_pRenderTargetView[ii] );
		I_RELEASE( m_stRenderTarget.m_pShaderResourceView[ii] );
		#if defined( USE_MSAA )
		I_RELEASE( m_stRenderTarget.m_pResolveTexture[ii] );
		I_RELEASE( m_stRenderTarget.m_pRenderTextureSRV[ii] );
		#endif
	}
	for( int ii = 0; ii < enWorkRTmax; ii++ )
	{
		I_RELEASE( m_pWorkTexture[ii] );
		I_RELEASE( m_pWorkRenderTargetView[ii] );
		I_RELEASE( m_pWorkShaderResourceView[ii] );
	}
}

HRESULT CShader::CreateSampler()
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	// SamplerState Wrap
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD			= 0;
	sampDesc.MaxLOD			= D3D11_FLOAT32_MAX;
	I_RETURN( pcDevice->m_pd3dDevice->CreateSamplerState( 
		&sampDesc, &m_pSamplerState[enState_Wrap] ) );

	// SamplerState Mirror
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_MIRROR;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_MIRROR;
	I_RETURN( pcDevice->m_pd3dDevice->CreateSamplerState( 
		&sampDesc, &m_pSamplerState[enState_Mirror] ) );

	return hr;
}

HRESULT CShader::CreateRenderTarget()
{
	HRESULT hr = S_OK;
	CDevice* pcDevice = CDevice::Instance();

	// 2�����e�N�X�`���̐ݒ�
	D3D11_TEXTURE2D_DESC texDesc;
	memset( &texDesc, 0, sizeof( texDesc ) );
	texDesc.Usage				= D3D11_USAGE_DEFAULT;
	texDesc.Format				= DXGI_FORMAT_R8G8B8A8_TYPELESS;
	texDesc.BindFlags			= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Width				= WIDTH;
	texDesc.Height				= HEIGHT;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;

	// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset( &rtvDesc, 0, sizeof( rtvDesc ) );
	rtvDesc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;

	// �V�F�[�_���\�[�X�r���[�̐ݒ�
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset( &srvDesc, 0, sizeof( srvDesc ) );
	srvDesc.Format				= rtvDesc.Format;
	srvDesc.Texture2D.MipLevels	= 1;

	for( int ii = enRT_GBStart; ii < enRT_GBEnd; ii++ )
	{
		texDesc.SampleDesc		= pcDevice->m_sampleDesc;
		rtvDesc.ViewDimension	= ( pcDevice->m_sampleDesc.Count > 1 )? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		srvDesc.ViewDimension	= ( pcDevice->m_sampleDesc.Count > 1 )? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

		// 2�����e�N�X�`���̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateTexture2D( 
			&texDesc, NULL, 
			&m_stRenderTarget.m_pTexture[ii] ),
			"Error : ID3D11Device::CreateTexture2D() Failed." );

		// �����_�[�^�[�Q�b�g�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateRenderTargetView( 
			m_stRenderTarget.m_pTexture[ii], &rtvDesc, 
			&m_stRenderTarget.m_pRenderTargetView[ii] ),
			"ID3D11Device::CreateRenderTargetView() Failed." );
	
		// �V�F�[�_���\�[�X�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateShaderResourceView( 
			m_stRenderTarget.m_pTexture[ii], &srvDesc,
			&m_stRenderTarget.m_pShaderResourceView[ii] ),
			"ID3D11Device::CreateShaderResourceView() Failed." );
	}

	for( int ii = enRT_SSStart; ii < enRT_SSEnd; ii++ )
	{
		texDesc.SampleDesc.Count	= 1;
		texDesc.SampleDesc.Quality	= 0;
		rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
		srvDesc.ViewDimension		= D3D11_SRV_DIMENSION_TEXTURE2D;

		// 2�����e�N�X�`���̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateTexture2D( 
			&texDesc, NULL, 
			&m_stRenderTarget.m_pTexture[ii] ),
			"Error : ID3D11Device::CreateTexture2D() Failed." );

		// �����_�[�^�[�Q�b�g�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateRenderTargetView( 
			m_stRenderTarget.m_pTexture[ii], &rtvDesc, 
			&m_stRenderTarget.m_pRenderTargetView[ii] ),
			"ID3D11Device::CreateRenderTargetView() Failed." );
	
		// �V�F�[�_���\�[�X�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateShaderResourceView( 
			m_stRenderTarget.m_pTexture[ii], &srvDesc,
			&m_stRenderTarget.m_pShaderResourceView[ii] ),
			"ID3D11Device::CreateShaderResourceView() Failed." );
	}

	for( int ii = enRT_ShadowStart; ii < enRT_ShadowEnd; ii++ )
	{
		texDesc.SampleDesc		= pcDevice->m_sampleDesc;
		rtvDesc.ViewDimension	= ( pcDevice->m_sampleDesc.Count > 1 )? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		srvDesc.ViewDimension	= ( pcDevice->m_sampleDesc.Count > 1 )? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

		// 2�����e�N�X�`���̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateTexture2D( 
			&texDesc, NULL, 
			&m_stRenderTarget.m_pTexture[ii] ),
			"Error : ID3D11Device::CreateTexture2D() Failed." );

		// �����_�[�^�[�Q�b�g�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateRenderTargetView( 
			m_stRenderTarget.m_pTexture[ii], &rtvDesc, 
			&m_stRenderTarget.m_pRenderTargetView[ii] ),
			"ID3D11Device::CreateRenderTargetView() Failed." );
	
		// �V�F�[�_���\�[�X�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateShaderResourceView( 
			m_stRenderTarget.m_pTexture[ii], &srvDesc,
			&m_stRenderTarget.m_pShaderResourceView[ii] ),
			"ID3D11Device::CreateShaderResourceView() Failed." );
	}

	for( int ii = 0; ii < enWorkRTmax; ii++ )
	{
		texDesc.SampleDesc.Count	= 1;
		texDesc.SampleDesc.Quality	= 0;
		rtvDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
		srvDesc.ViewDimension		= D3D11_SRV_DIMENSION_TEXTURE2D;

		// 2�����e�N�X�`���̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateTexture2D( 
			&texDesc, NULL, 
			&m_pWorkTexture[ii] ),
			"Error : ID3D11Device::CreateTexture2D() Failed." );

		// �����_�[�^�[�Q�b�g�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateRenderTargetView( 
			m_pWorkTexture[ii], &rtvDesc, 
			&m_pWorkRenderTargetView[ii] ),
			"ID3D11Device::CreateRenderTargetView() Failed." );
	
		// �V�F�[�_���\�[�X�r���[�̐���
		I_RETURN_M( pcDevice->m_pd3dDevice->CreateShaderResourceView( 
			m_pWorkTexture[ii], &srvDesc,
			&m_pWorkShaderResourceView[ii] ),
			"ID3D11Device::CreateShaderResourceView() Failed." );
	}

	#if defined( USE_MSAA )
	if ( pcDevice->m_sampleDesc.Count > 1 )
	{
		D3D11_TEXTURE2D_DESC descTex = texDesc;
		descTex.SampleDesc.Count = 1;
		descTex.SampleDesc.Quality = 0;

		D3D11_SHADER_RESOURCE_VIEW_DESC resolveSRVDesc;
		resolveSRVDesc.Format = rtvDesc.Format;
		resolveSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resolveSRVDesc.Texture2D.MostDetailedMip = 0;
		resolveSRVDesc.Texture2D.MipLevels = static_cast<UINT>(-1);

		for( int ii = 0; ii < enRT_Max; ii++ )
		{
			pcDevice->m_pd3dDevice->CreateTexture2D(
				&descTex, NULL, &m_stRenderTarget.m_pResolveTexture[ii] );

			pcDevice->m_pd3dDevice->CreateShaderResourceView(
				m_stRenderTarget.m_pResolveTexture[ii], &resolveSRVDesc, 
				&m_stRenderTarget.m_pRenderTextureSRV[ii] );
		}

		m_stRenderTarget.m_resolveFormat = rtvDesc.Format;
	}
	#endif

	return hr;
}

HRESULT CShader::CompileShaderFromFile(
	LPCWSTR szFileName,
	LPCSTR szEntryPoint,
	LPCSTR szShaderModel,
	ID3DBlob** ppBlobOut )
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

	#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3DCOMPILE_DEBUG;
	#endif

	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile( (LPCWSTR)szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
	if( FAILED(hr) )
	{
		if( pErrorBlob != NULL )
			OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
		if( pErrorBlob ) pErrorBlob->Release();
		return hr;
	}
	if( pErrorBlob ) pErrorBlob->Release();

	return S_OK;
}

HRESULT CShader::CreateVertexShader( char* _filename, LPCWSTR _filePath )
{
	HRESULT hr;
	CDevice* pcDevice = CDevice::Instance();

	// Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	I_RETURN_MFNC( CompileShaderFromFile( 
		_filePath, "VS", "vs_4_0", &pVSBlob ),
		"VertexShader not found.",
		pVSBlob->Release() );
 
	//���_�V�F�[�_�[����
	ID3D11VertexShader* pVertexShader;
	I_RETURN_FNC( pcDevice->m_pd3dDevice->CreateVertexShader(
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		nullptr,
		&pVertexShader ),
		pVSBlob->Release() );

	unsigned char* bufptr = (unsigned char*)pVSBlob->GetBufferPointer();
 
	//���͍\���̂̏�񂠂�ʒu�ւ̃|�C���^���擾
	unsigned char* pInStruct = nullptr;	//!<���͍\���̂̏�񂪂���ʒu�ւ̃|�C���^
	for( unsigned long ii = 0L; ii < pVSBlob->GetBufferSize() - 4; ii++ )
	{
		if( memcmp( bufptr + ii, "ISGN", 4 ) == NULL )
		{
			pInStruct = bufptr + ii;
			break;
		}
	}
	if( pInStruct == nullptr )
	{
		return FWP_E_NOT_FOUND;
	}
 
	//���͍\���̂̏����擾
	int cntvariable = pInStruct[8];						//!< �ϐ��̐�
	int cntvariablesystem = 0;							//!< �V�X�e���l�Z�}���e�B�N�X�����蓖�Ă�ꂽ�ϐ��̐�
	std::vector<char*> semanticsname( cntvariable );	//!< �Z�}���e�B�N�X��
	std::vector<UINT> semanticsindex( cntvariable );	//!< �Z�}���e�B�N�X�C���f�b�N�X
	std::vector<DXGI_FORMAT> format( cntvariable );		//!< �ϐ��̌^
	unsigned char *str=&pInStruct[16];
	for( int ii= 0; ii < cntvariable; ii++ )
	{
		semanticsname[ii] = (char*)( str[ii*24] + pInStruct + 8 );
		//�V�X�e���l�Z�}���e�B�N�X���t���Ă���ϐ��͖���
		if( strcmp( semanticsname[ii], "SV_InstanceID" ) == 0 )
		{
			cntvariablesystem++;
			continue;
		}
		semanticsindex[ii] = str[ii*24+4];
		switch( str[ii*24+20] )
		{
			//4����
			case '\x0f':
				switch( str[ii*24+12] )
				{
					case D3D_REGISTER_COMPONENT_FLOAT32:
						format[ii] = DXGI_FORMAT_R32G32B32A32_FLOAT;
						break;
					default:
						format[ii] = DXGI_FORMAT_UNKNOWN;
						break;
				}
				break;
			//3����
			case '\x07':
				switch( str[ii*24+12] )
				{
					case D3D_REGISTER_COMPONENT_FLOAT32:
						format[ii] = DXGI_FORMAT_R32G32B32_FLOAT;
						break;
					default:
						format[ii] = DXGI_FORMAT_UNKNOWN;
						break;
				}
				break;
			//2����
			case '\x03':
				switch( str[ii*24+12] )
				{
					case D3D_REGISTER_COMPONENT_FLOAT32:
						format[ii] = DXGI_FORMAT_R32G32_FLOAT;
						break;
					default:
						format[ii] = DXGI_FORMAT_UNKNOWN;
						break;
				}
				break;
			//1����
			case '\x01':
				switch( str[ii*24+12] )
				{
					case D3D_REGISTER_COMPONENT_FLOAT32:
						format[ii] = DXGI_FORMAT_R32_FLOAT;
						break;
					case D3D_REGISTER_COMPONENT_UINT32:
						format[ii] = DXGI_FORMAT_R32_UINT;
						break;
					default:
						format[ii] = DXGI_FORMAT_UNKNOWN;
						break;
				}
				break;
			default:
				format[ii] = DXGI_FORMAT_UNKNOWN;
				break;
		}
	}
	cntvariable -= cntvariablesystem;	//!< �ϐ��̐�����V�X�e���l�Z�}���e�B�N�X���t���Ă���ϐ��̐��������Ė����������̂Ƃ���
 
	//���̓��C�A�E�g�I�u�W�F�N�g���쐬
	std::vector<D3D11_INPUT_ELEMENT_DESC> hInElementDesc(cntvariable);
	for( int ii = 0; ii < cntvariable; ii++ )
	{
		hInElementDesc[ii].SemanticName			= semanticsname[ii];
		hInElementDesc[ii].SemanticIndex		= semanticsindex[ii];
		hInElementDesc[ii].Format				= format[ii];
		hInElementDesc[ii].InputSlot			= 0;
		hInElementDesc[ii].AlignedByteOffset	= D3D11_APPEND_ALIGNED_ELEMENT;
		hInElementDesc[ii].InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;
		hInElementDesc[ii].InstanceDataStepRate	= 0;
	}
 
	//���̓��C�A�E�g���쐬
	ID3D11InputLayout* pInputLayout = nullptr;
	I_RETURN( pcDevice->m_pd3dDevice->CreateInputLayout(
		hInElementDesc.data(),
		cntvariable,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&pInputLayout ) );

	// Set
	StVertexShader* pStVertexShader = I_NEW( StVertexShader );
	pStVertexShader->name = _filename;
	pStVertexShader->pInputLayout = pInputLayout;
	pStVertexShader->pVertexShader = pVertexShader;
	m_listVertexShader.push_back( pStVertexShader );

	// Relese
	pVSBlob->Release();

	return hr;
}

HRESULT CShader::CreatePixelShader( char* _filename, LPCWSTR _filePath )
{
	HRESULT hr;
	CDevice* pcDevice = CDevice::Instance();

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	I_RETURN_MFNC( CompileShaderFromFile( _filePath, "PS", "ps_4_0", &pPSBlob ),
		"PixelShader not found.", pPSBlob->Release() );

	// Create the pixel shader
	ID3D11PixelShader* pPixelShader = nullptr;
	I_RETURN_FNC( pcDevice->m_pd3dDevice->CreatePixelShader( 
		pPSBlob->GetBufferPointer(),
		pPSBlob->GetBufferSize(),
		NULL,
		&pPixelShader ),
		pPSBlob->Release() );

	// Set
	StPixelShader* pStPixelShader = I_NEW( StPixelShader );
	pStPixelShader->name = _filename;
	pStPixelShader->pPixelShader = pPixelShader;
	m_listPixelShader.push_back( pStPixelShader );

	// Relese
	pPSBlob->Release();

	return hr;
}

ID3D11InputLayout* CShader::GetInputLayout( char* _filename )
{
	std::string name = _filename;
	for( auto obj = m_listVertexShader.begin(); obj != m_listVertexShader.end(); obj++ )
	{
		if( *obj != nullptr )
		{
			if( name == (*obj)->name )
			{
				return (*obj)->pInputLayout;
			}
		}
	}
	return nullptr;
}

ID3D11VertexShader* CShader::GetVertexShader( char* _filename )
{
	std::string name = _filename;
	for( auto obj = m_listVertexShader.begin(); obj != m_listVertexShader.end(); obj++ )
	{
		if( *obj != nullptr )
		{
			if( name == (*obj)->name )
			{
				return (*obj)->pVertexShader;
			}
		}
	}
	return nullptr;
}

ID3D11PixelShader* CShader::GetPixelShader( char* _filename )
{
	std::string name = _filename;
	for( auto obj = m_listPixelShader.begin(); obj != m_listPixelShader.end(); obj++ )
	{
		if( *obj != nullptr )
		{
			if( name == (*obj)->name )
			{
				return (*obj)->pPixelShader;
			}
		}
	}
	return nullptr;
}

ID3D11SamplerState* CShader::GetSamplerState( EnState _en )
{
	if( _en >= enState_Max )
	{
		return nullptr;
	}
	return m_pSamplerState[_en];
}
