/* GraphicDemo class */

#ifndef H_CGRAPHICDEMO
#define H_CGRAPHICDEMO

#include "object\CSequence.h"

class CGBuffer;
class CShadowMap;
class CDeferred;
class CBlur;

class CGraphicsDemo : public CSequence
{
public:
	CGraphicsDemo();
	~CGraphicsDemo();
private:
	CGBuffer*	m_pcGBuffer;
	CShadowMap*	m_pcShadowMap;
	CDeferred*		m_pcSkin;
	CBlur*		m_pcBlur;
};

#endif
