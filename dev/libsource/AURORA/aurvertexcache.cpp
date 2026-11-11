#include "aurvertexcache.h"
// PC 0x004D6910
VertexCache::VertexCache(int nNumEntries)
{
    m_nNumEntries = nNumEntries;
    m_pEntries = new int[m_nNumEntries];
    for (int i = 0; i < m_nNumEntries; i++)
    {
        m_pEntries[i] = -1;
    }
}
// PC 0x004D6950
VertexCache::~VertexCache()
{
    delete[] m_pEntries;
}
