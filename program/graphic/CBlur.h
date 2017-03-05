/* Blur class */

#ifndef H_CBLUR
#define H_CBLUR

#include <d3d11.h>
#include "object\CRender.h"

class CBlur : public CRender
{
public:
	CBlur();
	~CBlur();
	void Render( ID3D11DeviceContext* _pContext );
private:
};

#endif
