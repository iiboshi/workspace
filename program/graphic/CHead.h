/* CHead class */

#ifndef H_CHEAD
#define H_CHEAD

#include "object\CModel.h"

class CHead : public CModel
{
public:
	CHead();
	~CHead();
	void Create();
	static unsigned int WINAPI Load( LPVOID lpParameter );
private:
	static HANDLE m_hThread;
};

#endif
