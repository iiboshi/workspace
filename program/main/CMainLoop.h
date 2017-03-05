/* main loop class */

#ifndef H_CMAINLOOP
#define H_CMAINLOOP

#include <vector>

class CSequence;
class CUpdate;
class CRender;


class CMainLoop
{
I_SINGLETON( CMainLoop )
public:
	CMainLoop();
	~CMainLoop();
	void Begine();
	void Release();
	int MainLoop();
	void SetSequence( CSequence* _obj );
	void AddUpdate( CUpdate* _obj );
	void AddRender( CRender* _obj );
private:
	CSequence* m_pcSequence;
	std::vector<CUpdate*> listUpdate;
	std::vector<CRender*> listRender;
};

#endif
