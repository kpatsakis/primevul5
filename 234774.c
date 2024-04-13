static TABLE_LIST* get_emb_subq(JOIN_TAB *tab)
{
  TABLE_LIST *tlist= tab->table->pos_in_table_list;
  if (tlist->jtbm_subselect)
    return tlist;
  TABLE_LIST *embedding= tlist->embedding;
  if (!embedding || !embedding->sj_subq_pred)
    return NULL;
  return embedding;
}