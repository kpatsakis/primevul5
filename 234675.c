void JOIN::save_query_plan(Join_plan_state *save_to)
{
  DYNAMIC_ARRAY tmp_keyuse;
  /* Swap the current and the backup keyuse internal arrays. */
  tmp_keyuse= keyuse;
  keyuse= save_to->keyuse; /* keyuse is reset to an empty array. */
  save_to->keyuse= tmp_keyuse;

  for (uint i= 0; i < table_count; i++)
  {
    save_to->join_tab_keyuse[i]= join_tab[i].keyuse;
    join_tab[i].keyuse= NULL;
    save_to->join_tab_checked_keys[i]= join_tab[i].checked_keys;
    join_tab[i].checked_keys.clear_all();
  }
  memcpy((uchar*) save_to->best_positions, (uchar*) best_positions,
         sizeof(POSITION) * (table_count + 1));
  memset((uchar*) best_positions, 0, sizeof(POSITION) * (table_count + 1));
  
  /* Save SJM nests */
  List_iterator<TABLE_LIST> it(select_lex->sj_nests);
  TABLE_LIST *tlist;
  SJ_MATERIALIZATION_INFO **p_info= save_to->sj_mat_info;
  while ((tlist= it++))
  {
    *(p_info++)= tlist->sj_mat_info;
  }
}