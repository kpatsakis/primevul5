uint check_join_cache_usage(JOIN_TAB *tab,
                            ulonglong options,
                            uint no_jbuf_after,
                            uint table_index,
                            JOIN_TAB *prev_tab)
{
  Cost_estimate cost;
  uint flags= 0;
  ha_rows rows= 0;
  uint bufsz= 4096;
  JOIN_CACHE *prev_cache=0;
  JOIN *join= tab->join;
  MEM_ROOT *root= join->thd->mem_root;
  uint cache_level= tab->used_join_cache_level;
  bool force_unlinked_cache=
         !(join->allowed_join_cache_types & JOIN_CACHE_INCREMENTAL_BIT);
  bool no_hashed_cache=
         !(join->allowed_join_cache_types & JOIN_CACHE_HASHED_BIT);
  bool no_bka_cache= 
         !(join->allowed_join_cache_types & JOIN_CACHE_BKA_BIT);

  join->return_tab= 0;

  /*
    Don't use join cache if @@join_cache_level==0 or this table is the first
    one join suborder (either at top level or inside a bush)
  */
  if (cache_level == 0 || !prev_tab)
    return 0;

  if (force_unlinked_cache && (cache_level%2 == 0))
    cache_level--;

  if (options & SELECT_NO_JOIN_CACHE)
    goto no_join_cache;

  if (tab->use_quick == 2)
    goto no_join_cache;

  if (tab->table->map & join->complex_firstmatch_tables)
    goto no_join_cache;
  
  /*
    Don't use join cache if we're inside a join tab range covered by LooseScan
    strategy (TODO: LooseScan is very similar to FirstMatch so theoretically it 
    should be possible to use join buffering in the same way we're using it for
    multi-table firstmatch ranges).
  */
  if (tab->inside_loosescan_range)
    goto no_join_cache;

  if (tab->is_inner_table_of_semijoin() &&
      !join->allowed_semijoin_with_cache)
    goto no_join_cache;
  if (tab->is_inner_table_of_outer_join() &&
      !join->allowed_outer_join_with_cache)
    goto no_join_cache;

  /*
    Non-linked join buffers can't guarantee one match
  */
  if (tab->is_nested_inner())
  {
    if (force_unlinked_cache || cache_level == 1)
      goto no_join_cache;
    if (cache_level & 1)
      cache_level--;
  }
    
  /*
    Don't use BKA for materialized tables. We could actually have a
    meaningful use of BKA when linked join buffers are used.

    The problem is, the temp.table is not filled (actually not even opened
    properly) yet, and this doesn't let us call
    handler->multi_range_read_info(). It is possible to come up with
    estimates, etc. without acessing the table, but it seems not to worth the
    effort now.
  */
  if (tab->table->pos_in_table_list->is_materialized_derived())
  {
    no_bka_cache= true;
    /*
      Don't use hash join algorithm if the temporary table for the rows
      of the derived table will be created with an equi-join key.
    */
    if (tab->table->s->keys)
      no_hashed_cache= true;
  }

  /*
    Don't use join buffering if we're dictated not to by no_jbuf_after
    (This is not meaningfully used currently)
  */
  if (table_index > no_jbuf_after)
    goto no_join_cache;
  
  /*
    TODO: BNL join buffer should be perfectly ok with tab->bush_children.
  */
  if (tab->loosescan_match_tab || tab->bush_children)
    goto no_join_cache;

  for (JOIN_TAB *first_inner= tab->first_inner; first_inner;
       first_inner= first_inner->first_upper)
  {
    if (first_inner != tab && 
        (!first_inner->use_join_cache || !(tab-1)->use_join_cache))
      goto no_join_cache;
  }
  if (tab->first_sj_inner_tab && tab->first_sj_inner_tab != tab &&
      (!tab->first_sj_inner_tab->use_join_cache || !(tab-1)->use_join_cache))
    goto no_join_cache;
  if (!prev_tab->use_join_cache)
  {
    /* 
      Check whether table tab and the previous one belong to the same nest of
      inner tables and if so do not use join buffer when joining table tab. 
    */
    if (tab->first_inner && tab != tab->first_inner)
    {
      for (JOIN_TAB *first_inner= tab[-1].first_inner;
           first_inner;
           first_inner= first_inner->first_upper)
      {
        if (first_inner == tab->first_inner)
          goto no_join_cache;
      }
    }
    else if (tab->first_sj_inner_tab && tab != tab->first_sj_inner_tab &&
             tab->first_sj_inner_tab == tab[-1].first_sj_inner_tab)
      goto no_join_cache; 
  }       

  prev_cache= prev_tab->cache;

  switch (tab->type) {
  case JT_ALL:
    if (cache_level == 1)
      prev_cache= 0;
    if ((tab->cache= new (root) JOIN_CACHE_BNL(join, tab, prev_cache)))
    {
      tab->icp_other_tables_ok= FALSE;
      return (2 - MY_TEST(!prev_cache));
    }
    goto no_join_cache;
  case JT_SYSTEM:
  case JT_CONST:
  case JT_REF:
  case JT_EQ_REF:
    if (cache_level <=2 || (no_hashed_cache && no_bka_cache))
      goto no_join_cache;
    if (tab->ref.is_access_triggered())
      goto no_join_cache;

    if (!tab->is_ref_for_hash_join() && !no_bka_cache)
    {
      flags= HA_MRR_NO_NULL_ENDPOINTS | HA_MRR_SINGLE_POINT;
      if (tab->table->covering_keys.is_set(tab->ref.key))
        flags|= HA_MRR_INDEX_ONLY;
      rows= tab->table->file->multi_range_read_info(tab->ref.key, 10, 20,
                                                    tab->ref.key_parts,
                                                    &bufsz, &flags, &cost);
    }

    if ((cache_level <=4 && !no_hashed_cache) || no_bka_cache ||
        tab->is_ref_for_hash_join() ||
	((flags & HA_MRR_NO_ASSOCIATION) && cache_level <=6))
    {
      if (!tab->hash_join_is_possible() ||
          tab->make_scan_filter())
        goto no_join_cache;
      if (cache_level == 3)
        prev_cache= 0;
      if ((tab->cache= new (root) JOIN_CACHE_BNLH(join, tab, prev_cache)))
      {
        tab->icp_other_tables_ok= FALSE;        
        return (4 - MY_TEST(!prev_cache));
      }
      goto no_join_cache;
    }
    if (cache_level > 4 && no_bka_cache)
      goto no_join_cache;
    
    if ((flags & HA_MRR_NO_ASSOCIATION) &&
	(cache_level <= 6 || no_hashed_cache))
      goto no_join_cache;

    if ((rows != HA_POS_ERROR) && !(flags & HA_MRR_USE_DEFAULT_IMPL))
    {
      if (cache_level <= 6 || no_hashed_cache)
      {
        if (cache_level == 5)
          prev_cache= 0;
        if ((tab->cache= new (root) JOIN_CACHE_BKA(join, tab, flags, prev_cache)))
          return (6 - MY_TEST(!prev_cache));
        goto no_join_cache;
      }
      else
      {
        if (cache_level == 7)
          prev_cache= 0;
        if ((tab->cache= new (root) JOIN_CACHE_BKAH(join, tab, flags, prev_cache)))
	{
          tab->idx_cond_fact_out= FALSE;
          return (8 - MY_TEST(!prev_cache));
        }
        goto no_join_cache;
      }
    }
    goto no_join_cache;
  default : ;
  }

no_join_cache:
  if (tab->type != JT_ALL && tab->is_ref_for_hash_join())
  {
    tab->type= JT_ALL;
    tab->ref.key_parts= 0;
  }
  revise_cache_usage(tab); 
  return 0;
}