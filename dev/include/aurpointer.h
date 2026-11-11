#ifndef SAFEPOINTER_H
#define SAFEPOINTER_H

#include <aurlist.h>

template <class T> class SafePointer
{
public:
    SafePointer()  // PC 0x00421C30
        : m_pObject(0)
    {
    }

#ifdef _XBOX
    SafePointer(T* pObject);
#else
    SafePointer(T* pObject)
        : m_pObject(pObject)
    {
        if (m_pObject)
        {
            m_pObject->m_lstSafePointers.Add(this);
        }
    }
#endif
#ifndef _XBOX
    SafePointer(const SafePointer& other) : m_pObject(other.m_pObject)
    {
        if (m_pObject) m_pObject->m_lstSafePointers.Add(this);
    }
#else
    SafePointer(const SafePointer& other);
#endif
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
#ifdef _XBOX
        m_pObject->RemoveSafePointer(this);
#else
        List<SafePointer<T>*>& list = m_pObject->m_lstSafePointers; list.Remove(this);
#endif
    }




    __forceinline void SetPointer(T* pObject)
    {
#ifdef _XBOX
        if (m_pObject) m_pObject->RemoveSafePointer(this);
        m_pObject = pObject; if (pObject) pObject->AddSafePointer(this);
#else
        List<SafePointer<T>*>* pList;
        if (m_pObject) {
            pList = &m_pObject->m_lstSafePointers;
            pList->Remove(this); }
        m_pObject = pObject; if (pObject) {
            pList = &pObject->m_lstSafePointers;
            pList->Add(this); }
#endif
    }

    T* m_pObject;
};



template <class T> class SafePointerList
{
public:
    SafePointerList() {}
#ifdef _XBOX
    void AddSafePointer(SafePointer<T>* pPointer); void RemoveSafePointer(SafePointer<T>* pPointer);
#endif
    ~SafePointerList();

    List<SafePointer<T>*> m_lstSafePointers;
};


#ifdef _XBOX
template <class T> SafePointer<T>::SafePointer(T* pObject)
    : m_pObject(pObject)
{
    if (m_pObject)
    {
        m_pObject->AddSafePointer(this);
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

#ifdef _XBOX


template <class T> void SafePointerList<T>::AddSafePointer(SafePointer<T>* pPointer)
{
    if (m_lstSafePointers.m_nAllocated == 0)
        m_lstSafePointers.allocate(2);
    m_lstSafePointers.Add(pPointer);
}



template <class T> SafePointer<T>::SafePointer(const SafePointer& other)
    : m_pObject(other.m_pObject)
{
    if (m_pObject)
        m_pObject->AddSafePointer(this);
}


template <class T> void SafePointerList<T>::RemoveSafePointer(SafePointer<T>* pPointer)
{
    m_lstSafePointers.Remove(pPointer);
}
#endif

#endif
