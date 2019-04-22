#ifndef __ROLE_H_INCLUDE__
#define __ROLE_H_INCLUDE__


#include <map>
#include <iostream>
#include "test/common/struct.h"

/// 导出对象模块

class CObj { // tolua_export
  public:
    void Reset() {
        m_llID = -1;
        memset( m_szName, 0, sizeof( m_szName ) );
    }

  private:
    long long           m_llID;
    char                m_szName[MAX_NAME_LEN];

  public:
// tolua_begin
    CObj() {
        Reset();
    }

    virtual ~CObj() {
        Reset();
    }
    void SetObjID( long long llID ) {
        m_llID = llID;
    }
    long long GetObjID() {
        return m_llID;
    }
    void GetObjID(long long& llID) {
        llID = m_llID;
    }
    void SetName( const char* szName ) {
        strcpy( m_szName, szName );
    }
    const char* GetName() {
        return m_szName;
    }
};
// tolua_end


class CRole : public CObj { // tolua_export
  public:
    void Reset() {
        m_nHP = 0;
        m_nMP = 0;
    }
  private:
    int     m_nHP;
    int     m_nMP;

    TaskInfoMap m_TaskInfoMap;
    SPos    m_stPos;

  public:
// tolua_begin
    CRole() {
        Reset();
    }
    virtual ~CRole() {
        Reset();
    }
    void SetMp( int nMp ) {
        m_nMP = nMp;
    }
    int GetMp() {
        return m_nMP;
    }

    void SetHp( int nHp ) {
        m_nHP = nHp;
    }
    int GetHp() {
        return m_nHP;
    }

    void AddMP( int nMp ) {
        m_nMP += nMp;
    }
    void AddHP( int nHp ) {
        m_nHP += nHp;
    }


    void SetPos( const SPos& stPos ) {
        m_stPos = stPos;
    }
    SPos& GetPos() {
        return m_stPos;
    }

    bool AcceptTask( int nTaskID );
    void FinishTask( int nTaskID );
};
// tolua_end

class CUser { // tolua_export
  private:
    std::string m_strName;
  public:
// tolua_begin

    void SetName( const char* name );
    const char* GetName();
};
// tolua_end

#endif // __ROLE_H_INCLUDE__

