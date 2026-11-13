#ifndef MATH3D_AURLIST_H
#define MATH3D_AURLIST_H



template <class T> class List
{
public:

    enum { INITIAL_ALLOCATION = 16 };





#ifdef _XBOX


    List(int nAllocated = 0);
#else
    List(T* pList = 0)
    {
        m_nCount = 0;
        m_nAllocated = 0;
        m_pList = pList;
    }

    List(int nAllocated);
#endif

    ~List();

#ifdef _XBOX
    List(List<T>& other);
#else
    List(List<T>& other)
    {
        m_nCount = 0;
        m_nAllocated = 0;
        m_pList = 0;
        for (int i = 0; i < other.m_nCount; i++)
        {
            Add(other.m_pList[i]);
        }
    }
#endif
    void Remove(T item);
    void allocate(int nAllocated);
    void SetSize(int nSize);
    void Add(T item);
    List<T>& operator=(List<T>& other);
    void AddUnique(T item);
    void Insert(T item, int nIndex);
    void DelIndex(int nIndex);

#ifdef _XBOX
    int IndexOf(T item);
#else
    int IndexOf(T item)
    {
        for (int i = 0; i < m_nCount; i++)
        {
            if (m_pList[i] == item)
            {
                return i;
            }
        }

        return -1;
    }
#endif
    // PC 0x00448F20 GOG 0x00449070
    T& operator[](int nIndex)
    {
        return m_pList[nIndex];
    }

#ifdef _XBOX
    int Contains(T item);
#else
    int Contains(T item)  // PC 0x004DD480 GOG 0x004DCB60
    {
        int nFound = 0;
        for (int i = 0; i < m_nCount; i++)
        {
            if (m_pList[i] == item)
            {
                nFound++;
            }
        }
        return nFound;
    }
#endif

    T* m_pList;
    int m_nCount;
    int m_nAllocated;
};


#ifdef _XBOX
template <class T> List<T>::List(List<T>& other)
{
    m_nCount = 0;
    m_nAllocated = 0;
    m_pList = 0;
    for (int i = 0; i < other.m_nCount; i++)
    {
        Add(other.m_pList[i]);
    }
}

template <class T> int List<T>::Contains(T item)
{
    int nFound = 0;
    for (int i = 0; i < m_nCount; i++)
    {
        if (m_pList[i] == item)
        {
            nFound++;
        }
    }
    return nFound;
}

template <class T> int List<T>::IndexOf(T item)
{
    for (int i = 0; i < m_nCount; i++)
    {
        if (m_pList[i] == item)
        {
            return i;
        }
    }

    return -1;
}
#endif
template <class T> List<T>::List(int nAllocated)
{
    m_nCount = 0;
    m_nAllocated = 0;
    m_pList = 0;
    if (nAllocated != 0)
    {
        allocate(nAllocated);
    }
}

template <class T> List<T>::~List()
{
    if (m_pList != 0)
    {
        delete[] m_pList;
    }
}
// PC 0x004673E0 GOG 0x004AB840
template <class T> void List<T>::Remove(T item)
{
    int i;
    for (i = m_nCount - 1; i >= 0; i--)
    {
        if (m_pList[i] == item)
        {
            break;
        }
    }
    DelIndex(i);
}

template <class T> void List<T>::allocate(int nAllocated)
{
    T* pOldList = m_pList;
    m_nAllocated = nAllocated;
    m_pList = new T[m_nAllocated];
    for (int i = 0; i < m_nCount; i++)
        m_pList[i] = pOldList[i];
    if (pOldList != 0)
        delete[] pOldList;
}

template <class T> void List<T>::SetSize(int nSize)
{
    if (nSize == 0)
    {
        if (m_pList != 0)
        {
            delete[] m_pList;
            m_pList = 0;
        }
        m_nAllocated = 0;
    }
    else
    {
        allocate(nSize);
    }
    m_nCount = nSize;
}

template <class T> void List<T>::Add(T item)
{
    if (m_nCount == m_nAllocated)
    {
        allocate(m_nAllocated ? m_nAllocated * 2 : INITIAL_ALLOCATION);
    }
    m_pList[m_nCount++] = item;
}

template <class T> List<T>& List<T>::operator=(List<T>& other)
{
    m_nCount = 0;
    for (int i = 0; i < other.m_nCount; i++)
    {
        Add(other.m_pList[i]);
    }
    return *this;
}

template <class T> void List<T>::AddUnique(T item)
{
    if (!Contains(item))
    {
#ifdef _XBOX
        Add(item);
#else
        if (m_nCount == m_nAllocated)
        {
            allocate(m_nAllocated ? m_nAllocated * 2 : INITIAL_ALLOCATION);
        }
        m_pList[m_nCount++] = item;
#endif
    }
}

template <class T> void List<T>::Insert(T item, int nIndex)
{
    int nLast = m_nCount;
#ifdef _XBOX
    Add(item);
#else
    if (m_nCount == m_nAllocated)
        allocate(m_nAllocated ? m_nAllocated * 2 : INITIAL_ALLOCATION);
    m_pList[m_nCount++] = item;
#endif
    for (; nLast > nIndex; nLast--)
        m_pList[nLast] = m_pList[nLast - 1];
    m_pList[nIndex] = item;
}

template <class T> void List<T>::DelIndex(int nIndex)
{
    m_nCount--;
    while (nIndex < m_nCount)
    {
        m_pList[nIndex] = m_pList[nIndex + 1];
        nIndex++;
    }
}

#endif
