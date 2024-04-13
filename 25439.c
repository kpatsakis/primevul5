int Item_func_nullif::compare()
{
  if (m_cache)
    m_cache->cache_value();
  return cmp.compare();
}