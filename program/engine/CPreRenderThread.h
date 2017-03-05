/* PreRenderThread class */

#ifndef H_CPRERENDERTHREAD
#define H_CPRERENDERTHREAD

#include <windows.h>
#include <d3d11.h>
#include <vector>
#include "object\CPreRender.h"

class CPreRenderThread
{
I_SINGLETON( CPreRenderThread )
public:
	enum EnMode
	{
		enMode_Max = 3,
	};
	CPreRenderThread();
	~CPreRenderThread();
	void DestroyRenderThreads();
	HRESULT InitRenderThreads();
	static unsigned int WINAPI RenderFunction( LPVOID lpParameter );
	void AddRenderThread( CPreRender* _obj );
public:
	int									m_iCnt;
	static bool							m_bLoop[enMode_Max];
	static ID3D11DeviceContext*			m_pDeviceContext[enMode_Max];
	static ID3D11CommandList*			m_pCommandList[enMode_Max];
	static int							m_iThreadData[enMode_Max];
	static HANDLE						m_hThread[enMode_Max];
	static HANDLE						m_hBeginEvent[enMode_Max];
	static HANDLE						m_hEndEvent[enMode_Max];
	static std::vector<CPreRender*>		m_listRendrThread;
};

#endif
