static bool check_interleaving_with_nj(JOIN_TAB *next_tab)
{
  TABLE_LIST *next_emb= next_tab->table->pos_in_table_list->embedding;
  JOIN *join= next_tab->join;

  if (join->cur_embedding_map & ~next_tab->embedding_map)
  {
    /* 
      next_tab is outside of the "pair of brackets" we're currently in.
      Cannot add it.
    */
    return TRUE;
  }
   
  /*
    Do update counters for "pairs of brackets" that we've left (marked as
    X,Y,Z in the above picture)
  */
  for (;next_emb && next_emb != join->emb_sjm_nest; next_emb= next_emb->embedding)
  {
    if (!next_emb->sj_on_expr)
    {
      next_emb->nested_join->counter++;
      if (next_emb->nested_join->counter == 1)
      {
        /* 
          next_emb is the first table inside a nested join we've "entered". In
          the picture above, we're looking at the 'X' bracket. Don't exit yet as
          X bracket might have Y pair bracket.
        */
        join->cur_embedding_map |= next_emb->nested_join->nj_map;
      }
      
      if (next_emb->nested_join->n_tables !=
          next_emb->nested_join->counter)
        break;

      /*
        We're currently at Y or Z-bracket as depicted in the above picture.
        Mark that we've left it and continue walking up the brackets hierarchy.
      */
      join->cur_embedding_map &= ~next_emb->nested_join->nj_map;
    }
  }
  return FALSE;
}