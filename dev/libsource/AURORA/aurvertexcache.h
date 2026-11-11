#ifndef AURVERTEXCACHE_H
#define AURVERTEXCACHE_H

class VertexCache
{
public:
    VertexCache(int nNumEntries = 16);
    ~VertexCache();

    bool InCache(int nEntry)
    {
        for (int i = 0; i < m_nNumEntries; i++)
        {
            if (m_pEntries[i] == nEntry)
            {
                return true;
            }
        }
        return false;
    }

    int AddEntry(int nEntry)
    {
        int nRemoved = m_pEntries[m_nNumEntries - 1];
        for (int i = m_nNumEntries - 2; i >= 0; i--)
        {
            m_pEntries[i + 1] = m_pEntries[i];
        }
        m_pEntries[0] = nEntry;
        return nRemoved;
    }

private:

    int* m_pEntries;
    int m_nNumEntries;
};

#endif
