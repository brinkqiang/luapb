
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __DMSINGLETON_H_INCLUDE__
#define __DMSINGLETON_H_INCLUDE__

#include <assert.h>

#include <vector>
#include <algorithm>
#include <mutex>

template<typename T>
class TSingleton
{
private:
    typedef T  SingletonObj;
public:
    static SingletonObj* Instance()
    {
        static SingletonObj s_oT;
        return &s_oT;
    }
};

template<typename T>
class CDMSafeSingleton
{
private:
    typedef T  SingletonObj;
public:
    class  TSafeCreator
    {
    public:
        TSafeCreator()
        {
            CDMSafeSingleton<SingletonObj>::Instance();
        }
        inline void Do() {}
    };

    static TSafeCreator s_oCreator;
public:
    static SingletonObj* Instance()
    {
        static SingletonObj s_oT;
        s_oCreator.Do();
        return &s_oT;
    }

};

template <class T> typename
CDMSafeSingleton<T>::TSafeCreator CDMSafeSingleton<T>::s_oCreator;

class IDMSafeSingleton
{
public:
    virtual ~IDMSafeSingleton() = 0;
    virtual bool Init() = 0;
    virtual bool UnInit() = 0;
    virtual void Release() = 0;
};

inline IDMSafeSingleton::~IDMSafeSingleton() {}

class CDMSingletonFrame
{
public:
    typedef std::vector<IDMSafeSingleton*> VecSafeSafeSingleton;
    typedef VecSafeSafeSingleton::iterator VecSafeSafeSingletonIt;
    typedef VecSafeSafeSingleton::reverse_iterator VecSafeSafeSingletonRIt;
public:
    CDMSingletonFrame() {}
    virtual ~CDMSingletonFrame()
    {
        Release();
    }

    void AddSingleton(IDMSafeSingleton* pSink)
    {
        if (std::count(m_vecList.begin(), m_vecList.end(), pSink))
        {
            assert(0);
            return;
        }

        m_vecList.push_back(pSink);
    }

    static CDMSingletonFrame* Instance()
    {
        static CDMSingletonFrame s_oFrame;
        return &s_oFrame;
    }

    void Init()
    {
        for (VecSafeSafeSingletonIt It = m_vecList.begin(); It != m_vecList.end();
            ++It)
        {
            if (!(*It)->Init())
            {
                assert(0);
            }
        }
    }

    void UnInit()
    {
        for (VecSafeSafeSingletonRIt It = m_vecList.rbegin(); It != m_vecList.rend();
            ++It)
        {
            if (!(*It)->UnInit())
            {
                assert(0);
            }
        }
    }

    void Release()
    {
        for (VecSafeSafeSingletonRIt It = m_vecList.rbegin(); It != m_vecList.rend();
            ++It)
        {
            (*It)->Release();
        }

        m_vecList.clear();
    }

private:
    std::vector<IDMSafeSingleton*> m_vecList;
};

template<typename T>
class CDMSingleton : public IDMSafeSingleton
{
public:
    typedef T  SingletonObj;

    CDMSingleton()
    {
        CDMSingletonFrame::Instance()->AddSingleton(this);
    }
    virtual ~CDMSingleton() {}
public:
    class  TSafeCreator
    {
    public:
        TSafeCreator()
        {
            CDMSingleton<SingletonObj>::Create();
        }
        inline void Do() {}
    };

    static TSafeCreator s_oCreator;
public:
    static bool Create()
    {
        s_oCreator.Do();
        std::call_once(m_oOnce, [&]()
            {
                m_poInstance = new SingletonObj();
            });
        return NULL != m_poInstance;
    }

    static void Destroy()
    {
        s_oCreator.Do();
        delete m_poInstance;
        m_poInstance = NULL;
    }

    static T* Instance()
    {
        s_oCreator.Do();
        return m_poInstance;
    }

    virtual bool Init()
    {
        return true;
    }
    virtual bool UnInit()
    {
        return true;
    }

    virtual void Release()
    {
        Destroy();
    }
private:
    static std::once_flag m_oOnce;
    static SingletonObj* m_poInstance;
};

template <typename T>
std::once_flag CDMSingleton<T>::m_oOnce;

template<typename T>
T* CDMSingleton<T>::m_poInstance = NULL;

template <class T> typename
CDMSingleton<T>::TSafeCreator CDMSingleton<T>::s_oCreator;

#endif // __DMSINGLETON_H_INCLUDE__
