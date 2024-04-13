static void restore_prev_nj_state(JOIN_TAB *last)
{
  TABLE_LIST *last_emb= last->table->pos_in_table_list->embedding;
  JOIN *join= last->join;
  for (;last_emb != NULL && last_emb != join->emb_sjm_nest; 
       last_emb= last_emb->embedding)
  {
    if (!last_emb->sj_on_expr)
    {
      NESTED_JOIN *nest= last_emb->nested_join;
      DBUG_ASSERT(nest->counter > 0);
      
      bool was_fully_covered= nest->is_fully_covered();
      
      join->cur_embedding_map|= nest->nj_map;

      if (--nest->counter == 0)
        join->cur_embedding_map&= ~nest->nj_map;
      
      if (!was_fully_covered)
        break;
    }
  }
}