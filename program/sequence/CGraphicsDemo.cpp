/* GraphicDemo class */

#include "CGraphicsDemo.h"
#include "graphic\CSky.h"
#include "graphic\CGround.h"
#include "graphic\CBox.h"
#include "graphic\CHeadBox.h"
#include "graphic\CHead.h"
#include "graphic\CModelManager.h"
#include "graphic\CGBuffer.h"
#include "graphic\CShadow.h"
#include "graphic\CSSAO.h"
#include "graphic\CDeferred.h"
#include "graphic\CSSSS.h"
#include "graphic\CBlur.h"
#include "engine\CShader.h"

CGraphicsDemo::CGraphicsDemo()
	: m_pcGBuffer	( nullptr )
	, m_pCShadow	( nullptr )
	, m_pcSSAO		( nullptr )
	, m_pcDeferred	( nullptr )
	, m_pcSSSS		( nullptr )
	, m_pcBlur		( nullptr )
{
	// Common Shader.
	CShader::Instance()->CreateShader( "depthmap", SHADE_RPATH( DepthMap ) );
	CShader::Instance()->CreateShader( "commonblurx", SHADE_RPATH( CommonBlurX ) );
	CShader::Instance()->CreateShader( "commonblury", SHADE_RPATH( CommonBlurY ) );

	CModelManager::Instance()->m_listModel.push_back( I_NEW( CSky ) );
	CModelManager::Instance()->m_listModel.push_back( I_NEW( CGround ) );
	CModelManager::Instance()->m_listModel.push_back( I_NEW( CBox ) );
	CModelManager::Instance()->m_listModel.push_back( I_NEW( CHeadBox ) );
	CModelManager::Instance()->m_listModel.push_back( I_NEW( CHead ) );

	#if defined( USE_SHADOW )
	m_pCShadow	= I_NEW( CShadow );
	#endif
	#if defined( USE_AO )
	m_pcSSAO		= I_NEW( CSSAO );
	#endif
	#if !defined( SHADOWMAPTEST ) && !defined( SHADOWTEST ) && !defined( SHADOWBLURTEST ) && !defined( AOMAPTEST ) && !defined( AOTEST ) && !defined( AOBLURTEST )
	m_pcGBuffer		= I_NEW( CGBuffer );
	m_pcDeferred	= I_NEW( CDeferred );
	m_pcSSSS		= I_NEW( CSSSS );
	m_pcBlur		= I_NEW( CBlur );
	#endif
}

CGraphicsDemo::~CGraphicsDemo()
{
	I_DELETE( m_pcGBuffer );
	I_DELETE( m_pCShadow );
	I_DELETE( m_pcSSAO );
	I_DELETE( m_pcDeferred );
	I_DELETE( m_pcSSSS );
	I_DELETE( m_pcBlur );
}
