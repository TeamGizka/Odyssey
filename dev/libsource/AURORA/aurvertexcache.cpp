#include "aurvertexcache.h"
// PC 0x004A9680
VertexCache::VertexCache(int nNumEntries)
{
    m_nNumEntries = nNumEntries;
    m_pEntries = new int[m_nNumEntries];
    for (int i = 0; i < m_nNumEntries; i++)
    {
        m_pEntries[i] = -1;
    }
}
// PC 0x004A96C0
VertexCache::~VertexCache()
{
    delete[] m_pEntries;
}
