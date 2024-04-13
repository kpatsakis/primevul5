void JOIN::restore_query_plan(Join_plan_state *restore_from)
{
  DYNAMIC_ARRAY tmp_keyuse;
  tmp_keyuse= keyuse;
  keyuse= restore_from->keyuse;
  restore_from->keyuse= tmp_keyuse;

  for (uint i= 0; i < table_count; i++)
  {
    join_tab[i].keyuse= restore_from->join_tab_keyuse[i];
    join_tab[i].checked_keys= restore_from->join_tab_checked_keys[i];
  }

  memcpy((uchar*) best_positions, (uchar*) restore_from->best_positions,
         sizeof(POSITION) * (table_count + 1));
  /* Restore SJM nests */
  List_iterator<TABLE_LIST> it(select_lex->sj_nests);
  TABLE_LIST *tlist;
  SJ_MATERIALIZATION_INFO **p_info= restore_from->sj_mat_info;
  while ((tlist= it++))
  {
    tlist->sj_mat_info= *(p_info++);
  }
}