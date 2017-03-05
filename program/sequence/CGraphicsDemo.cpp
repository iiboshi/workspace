/* GraphicDemo class */

#include "CGraphicsDemo.h"
#include "graphic\CGBuffer.h"
#include "graphic\CShadowMap.h"
#include "graphic\CDeferred.h"
#include "graphic\CSSSS.h"
#include "graphic\CBlur.h"

CGraphicsDemo::CGraphicsDemo()
	: m_pcGBuffer	( nullptr )
	, m_pcShadowMap	( nullptr )
	, m_pcDeferred	( nullptr )
	, m_pcBlur		( nullptr )
{
	m_pcGBuffer		= I_NEW( CGBuffer );
	m_pcShadowMap	= I_NEW( CShadowMap );
	m_pcDeferred	= I_NEW( CDeferred );
	m_pcSSSS		= I_NEW( CSSSS );
	m_pcBlur		= I_NEW( CBlur );
}

CGraphicsDemo::~CGraphicsDemo()
{
	I_DELETE( m_pcGBuffer );
	I_DELETE( m_pcShadowMap );
	I_DELETE( m_pcDeferred );
	I_DELETE( m_pcSSSS );
	I_DELETE( m_pcBlur );
}
