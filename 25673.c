JOIN::destroy()
{
  DBUG_ENTER("JOIN::destroy");
  select_lex->join= 0;

  cond_equal= 0;
  having_equal= 0;

  cleanup(1);

  if (join_tab)
  {
    for (JOIN_TAB *tab= first_linear_tab(this, WITH_BUSH_ROOTS,
                                         WITH_CONST_TABLES);
         tab; tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
    {
      if (tab->aggr)
      {
        free_tmp_table(thd, tab->table);
        delete tab->tmp_table_param;
        tab->tmp_table_param= NULL;
        tab->aggr= NULL;
      }
      tab->table= NULL;
    }
  }

  /* Cleanup items referencing temporary table columns */
  cleanup_item_list(tmp_all_fields1);
  cleanup_item_list(tmp_all_fields3);
  destroy_sj_tmp_tables(this);
  delete_dynamic(&keyuse); 
  delete procedure;
  DBUG_RETURN(error);
}