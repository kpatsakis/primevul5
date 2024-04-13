void set_join_cache_denial(JOIN_TAB *join_tab)
{
  if (join_tab->cache)
  {
    /* 
      If there is a previous cache linked to this cache through the
      next_cache pointer: remove the link. 
    */
    if (join_tab->cache->prev_cache)
      join_tab->cache->prev_cache->next_cache= 0;
    /*
      Same for the next_cache
    */
    if (join_tab->cache->next_cache)
      join_tab->cache->next_cache->prev_cache= 0;

    join_tab->cache->free();
    join_tab->cache= 0;
  }
  if (join_tab->use_join_cache)
  {
    join_tab->use_join_cache= FALSE;
    join_tab->used_join_cache_level= 0;
    /*
      It could be only sub_select(). It could not be sub_seject_sjm because we
      don't do join buffering for the first table in sjm nest. 
    */
    join_tab[-1].next_select= sub_select;
    if (join_tab->type == JT_REF && join_tab->is_ref_for_hash_join())
    {
      join_tab->type= JT_ALL;
      join_tab->ref.key_parts= 0;
    }
    join_tab->join->return_tab= join_tab;
  }
}