#ifndef MATH3D_AURLIST_H
#define MATH3D_AURLIST_H



template <class T> class List
{
public:

#ifdef _XBOX
    enum { INITIAL_ALLOCATION = 16 };
#else
    enum { INITIAL_ALLOCATION = 8 };
#endif





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
    // PC 0x00437C40
    T& operator[](int nIndex)
    {
        return m_pList[nIndex];
    }

#ifdef _XBOX
    int Contains(T item);
#else
    int Contains(T item)  // PC 0x004AF240
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
// PC 0x004AF240
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
// PC 0x00445080
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
// PC 0x0046BD10  Xbox 0x002BA4E0
template <class T> void List<T>::AddUnique(T item)
{
    if (!Contains(item))
    {
        if (m_nCount == m_nAllocated)
        {
            allocate(m_nAllocated ? m_nAllocated * 2 : INITIAL_ALLOCATION);
        }
        m_pList[m_nCount++] = item;
    }
}

template <class T> void List<T>::Insert(T item, int nIndex)
{
    int nLast = m_nCount;
    if (m_nCount == m_nAllocated)
        allocate(m_nAllocated ? m_nAllocated * 2 : INITIAL_ALLOCATION);
    m_pList[m_nCount++] = item;
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
