/* ShadowMap class */

#ifndef H_CSHADOWMAP
#define H_CSHADOWMAP

#include <d3d11.h>
#include "object\CPreRender.h"

class CShadowMap : public CPreRender
{
public:
	CShadowMap();
	~CShadowMap();
	void Render( ID3D11DeviceContext* _pContext );
private:
};

#endif
