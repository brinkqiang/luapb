#include "rolemgr.h"
#include "role.h"

CRoleMgr::CRoleMgr(): m_llObjID( 10000 ) {
}

CRoleMgr::~CRoleMgr() {
}

CRole* CRoleMgr::CreateRole() {
    CRole* poRole = m_oRolePool.FetchObj();

    if ( NULL == poRole ) {
        return NULL;
    }

    poRole->SetObjID( GetNextObjID() );
    m_mapRole[poRole->GetObjID()] = poRole;
    return poRole;
}

void CRoleMgr::ReleaseRole( CRole* poRole ) {
    if ( NULL == poRole ) {
        return;
    }

    m_mapRole.erase( poRole->GetObjID() );
    m_oRolePool.ReleaseObj( poRole );
}

CRole* CRoleMgr::FindRole( long long llID ) {
    MapRoleIt It = m_mapRole.find( llID );

    if ( It == m_mapRole.end() ) {
        return NULL;
    }

    return It->second;
}

long long CRoleMgr::GetNextObjID() {
    for ( ;; ) {
        CRole* poRole = FindRole( ++m_llObjID );

        if ( poRole ) {
            continue;
        }

        break;
    }

    return m_llObjID;
}

