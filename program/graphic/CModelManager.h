/* ModelManager class */

#ifndef H_CMODELMANAGER
#define H_CMODELMANAGER

#include "object\CModel.h"
#include <vector>

class CModelManager
{
I_SINGLETON( CModelManager )
public:
	CModelManager();
	~CModelManager();
public:
	std::vector<CModel*> m_listModel;
};

#endif
