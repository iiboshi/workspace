/* ModelManager class */

#include "CModelManager.h"

CModelManager::CModelManager()
{
}

CModelManager::~CModelManager()
{
	I_LISTDELETE( m_listModel );
	m_listModel.clear();
}
