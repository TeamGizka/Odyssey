#ifndef SAFEPOINTER_H
#define SAFEPOINTER_H

#include <aurlist.h>

template <class T> class SafePointer
{
public:
    SafePointer()  // PC 0x004204E0  Xbox 0x0027F830
        : m_pObject(0)
    {
    }

#ifdef _XBOX
    SafePointer(T* pObject);
#else
    SafePointer(T* pObject)  // Xbox 0x000B05D0
        : m_pObject(pObject)
    {
        if (m_pObject)
        {
            m_pObject->m_lstSafePointers.Add(this);
        }
    }
#endif

    SafePointer(const SafePointer& other)
        : m_pObject(other.m_pObject)
    {
        if (m_pObject)
            m_pObject->m_lstSafePointers.Add(this);
    }

    ~SafePointer();

    SafePointer& operator=(T* pObject);





    SafePointer& operator=(const SafePointer& other)
    {
        return operator=(other.m_pObject);
    }

    T* operator->() const
    {
        return m_pObject;
    }

    operator T*() const
    {
        return m_pObject;
    }




    void RemovePointer()
    {
        List<SafePointer<T>*>& list = m_pObject->m_lstSafePointers;
        list.Remove(this);
    }







    __forceinline void SetPointer(T* pObject)
    {
        List<SafePointer<T>*>* pList;
        if (m_pObject)
        {
            pList = &m_pObject->m_lstSafePointers;
            pList->Remove(this);
        }
        m_pObject = pObject;
        if (pObject)
        {
            pList = &pObject->m_lstSafePointers;
            pList->Add(this);
        }
    }

    T* m_pObject;
};



template <class T> class SafePointerList
{
public:
    SafePointerList()
    {
    }

    ~SafePointerList();

    List<SafePointer<T>*> m_lstSafePointers;
};


#ifdef _XBOX
template <class T> SafePointer<T>::SafePointer(T* pObject)
    : m_pObject(pObject)
{
    if (m_pObject)
    {
        m_pObject->m_lstSafePointers.Add(this);
    }
}

template <class T> SafePointer<T>::~SafePointer()
#else
template <class T> inline SafePointer<T>::~SafePointer()
#endif
{
    if (m_pObject)
    {
        RemovePointer();
    }
}

#ifdef _XBOX
template <class T> SafePointer<T>& SafePointer<T>::operator=(T* pObject)
#else
template <class T> inline SafePointer<T>& SafePointer<T>::operator=(T* pObject)
#endif
{
    SetPointer(pObject);
    return *this;
}



template <class T> inline void ClearSafePointer(SafePointer<T>* pPointer)
{
    *pPointer = 0;
}

template <class T> SafePointerList<T>::~SafePointerList()
{
    for (int i = m_lstSafePointers.m_nCount - 1; i >= 0; i--)
    {
        ClearSafePointer(m_lstSafePointers[i]);
    }
    m_lstSafePointers.SetSize(0);
}

#endif
