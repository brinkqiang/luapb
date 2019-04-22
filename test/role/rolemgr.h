#ifndef __ROLEMGR_H_INCLUDE__
#define __ROLEMGR_H_INCLUDE__

#include <map>

#include "dmsingleton.h"
#include "dmrapidpool.h"
#include "test/role/role.h"

class CRoleMgr : public TSingleton<CRoleMgr> {
    friend class TSingleton<CRoleMgr>;

  protected:
    CRoleMgr();
    ~CRoleMgr();

  public:
    CRole*   FindRole( long long llID );
    CRole*   CreateRole();                   /// ����Ҫ������lua
    void     ReleaseRole( CRole* poRole );    /// ����Role,Ҫ��lua��ע��

  private:
    long long GetNextObjID();

    CDynamicRapidPool<CRole, 10000, 100> m_oRolePool;

    typedef std::map<long long, CRole*> MapRole;
    typedef std::map<long long, CRole*>::iterator MapRoleIt;
    MapRole m_mapRole;

    long long  m_llObjID;
};

#endif // __ROLEMGR_H_INCLUDE__
