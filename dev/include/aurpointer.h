#ifndef SAFEPOINTER_H
#define SAFEPOINTER_H

#include <aurlist.h>

template <class T> class SafePointer
{
public:
    SafePointer()  // PC 0x00421C30 GOG 0x00421C40
        : m_pObject(0)
    {
    }

    SafePointer(T* pObject);
    SafePointer(const SafePointer& other);
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
        m_pObject->RemoveSafePointer(this);
    }




    __forceinline void SetPointer(T* pObject)
    {
        if (m_pObject) m_pObject->RemoveSafePointer(this);
        m_pObject = pObject; if (pObject) pObject->AddSafePointer(this);
    }

    T* m_pObject;
};



template <class T> class SafePointerList
{
public:
    SafePointerList() {}
    void AddSafePointer(SafePointer<T>* pPointer); void RemoveSafePointer(SafePointer<T>* pPointer);
    ~SafePointerList();

    List<SafePointer<T>*> m_lstSafePointers;
};


template <class T> SafePointer<T>::SafePointer(T* pObject)
    : m_pObject(pObject)
{
    if (m_pObject)
    {
        m_pObject->AddSafePointer(this);
    }
}

template <class T> SafePointer<T>::~SafePointer()
{
    if (m_pObject)
    {
        RemovePointer();
    }
}

template <class T> SafePointer<T>& SafePointer<T>::operator=(T* pObject)
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
