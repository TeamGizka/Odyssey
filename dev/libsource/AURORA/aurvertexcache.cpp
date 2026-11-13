#include "aurvertexcache.h"
// PC 0x004D6910 GOG 0x004D6A50
VertexCache::VertexCache(int nNumEntries)
{
    m_nNumEntries = nNumEntries;
    m_pEntries = new int[m_nNumEntries];
    for (int i = 0; i < m_nNumEntries; i++)
    {
        m_pEntries[i] = -1;
    }
}
// PC 0x004D6950 GOG 0x004D6A90
VertexCache::~VertexCache()
{
    delete[] m_pEntries;
}
