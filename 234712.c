bool JOIN::shrink_join_buffers(JOIN_TAB *jt, 
                               ulonglong curr_space,
                               ulonglong needed_space)
{
  JOIN_TAB *tab;
  JOIN_CACHE *cache;
  for (tab= first_linear_tab(this, WITHOUT_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab != jt;
       tab= next_linear_tab(this, tab, WITHOUT_BUSH_ROOTS))
  {
    cache= tab->cache;
    if (cache)
    { 
      size_t buff_size;
      if (needed_space < cache->get_min_join_buffer_size())
        return TRUE;
      if (cache->shrink_join_buffer_in_ratio(curr_space, needed_space))
      { 
        revise_cache_usage(tab);
        return TRUE;
      }
      buff_size= cache->get_join_buffer_size();
      curr_space-= buff_size;
      needed_space-= buff_size;
    }
  }

  cache= jt->cache;
  DBUG_ASSERT(cache);
  if (needed_space < cache->get_min_join_buffer_size())
    return TRUE;
  cache->set_join_buffer_size((size_t)needed_space);
  
  return FALSE;
}