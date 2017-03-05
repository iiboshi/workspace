/* render class */

#ifndef H_CRENDER
#define H_CRENDER

#include <d3d11.h>

class CRender
{
public:
	CRender();
	virtual ~CRender();
	virtual void Render( ID3D11DeviceContext* _pContext ) = 0;
private:
};

#endif
