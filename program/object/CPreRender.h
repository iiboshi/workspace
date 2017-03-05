/* RenderThread class */

#ifndef H_CRENDERTHREAD
#define H_CRENDERTHREAD

#include <d3d11.h>

class CPreRender
{
public:
	CPreRender();
	virtual ~CPreRender();
	virtual void Render( ID3D11DeviceContext* _pContext ) = 0;
private:
};

#endif
