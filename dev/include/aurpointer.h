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
        m_pObject->RemovePointer(this);
    }





    T* m_pObject;
};



template <class T> class SafePointerList
{
public:  // Xbox 0x0021F750
    SafePointerList() {}
    void AddPointer(SafePointer<T>* pPointer); void RemovePointer(SafePointer<T>* pPointer);
    ~SafePointerList();

    List<SafePointer<T>*> m_lstSafePointers;
};


template <class T> SafePointer<T>::SafePointer(T* pObject)  // Xbox 0x000B1070
    : m_pObject(pObject)
{
    if (m_pObject)
    {
        m_pObject->AddPointer(this);
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
    if (m_pObject)
    {
        RemovePointer();
    }
    m_pObject = pObject;
    if (m_pObject)
    {
        m_pObject->AddPointer(this);
    }
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

template <class T> void SafePointerList<T>::AddPointer(SafePointer<T>* pPointer)
{
    if (m_lstSafePointers.m_nAllocated == 0)
        m_lstSafePointers.allocate(2);
    m_lstSafePointers.Add(pPointer);
}



template <class T> SafePointer<T>::SafePointer(const SafePointer& other)  // Xbox 0x002E9F30
    : m_pObject(other.m_pObject)
{
    if (m_pObject)
        m_pObject->AddPointer(this);
}


template <class T> void SafePointerList<T>::RemovePointer(SafePointer<T>* pPointer)
{
    m_lstSafePointers.Remove(pPointer);
}

#endif
