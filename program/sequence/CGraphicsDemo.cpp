/* GraphicDemo class */

#include "CGraphicsDemo.h"
#include "graphic\CGround.h"
#include "graphic\CBox.h"
#include "graphic\CHead.h"
#include "graphic\CModelManager.h"
#include "graphic\CGBuffer.h"
#include "graphic\CShadowMap.h"
#include "graphic\CDeferred.h"
#include "graphic\CSSSS.h"
#include "graphic\CBlur.h"

CGraphicsDemo::CGraphicsDemo()
	: m_pcGBuffer	( nullptr )
	, m_pcShadowMap	( nullptr )
	, m_pcDeferred	( nullptr )
	, m_pcSSSS		( nullptr )
	, m_pcBlur		( nullptr )
{
	CModelManager::Instance()->m_listModel.push_back( I_NEW( CGround ) );
	CModelManager::Instance()->m_listModel.push_back( I_NEW( CBox ) );
	CModelManager::Instance()->m_listModel.push_back( I_NEW( CHead ) );

	#if defined( USE_SHADOW )
	m_pcShadowMap	= I_NEW( CShadowMap );
	#endif
	#if !defined( SHADOWMAPTEST ) && !defined( SHADOWTEST ) && !defined( SHADOWBLURTEST )
	m_pcGBuffer		= I_NEW( CGBuffer );
	m_pcDeferred	= I_NEW( CDeferred );
	m_pcSSSS		= I_NEW( CSSSS );
	m_pcBlur		= I_NEW( CBlur );
	#endif
}

CGraphicsDemo::~CGraphicsDemo()
{
	I_DELETE( m_pcGBuffer );
	I_DELETE( m_pcShadowMap );
	I_DELETE( m_pcDeferred );
	I_DELETE( m_pcSSSS );
	I_DELETE( m_pcBlur );
}
