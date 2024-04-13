bool JOIN::add_having_as_table_cond(JOIN_TAB *tab)
{
  tmp_having->update_used_tables();
  table_map used_tables= tab->table->map | OUTER_REF_TABLE_BIT;

  /* If tmp table is not used then consider conditions of const table also */
  if (!need_tmp)
    used_tables|= const_table_map;

  DBUG_ENTER("JOIN::add_having_as_table_cond");

  Item* sort_table_cond= make_cond_for_table(thd, tmp_having, used_tables,
                                             (table_map) 0, false,
                                             false, false);
  if (sort_table_cond)
  {
    if (!tab->select)
    {
      if (!(tab->select= new SQL_SELECT))
        DBUG_RETURN(true);
      tab->select->head= tab->table;
    }
    if (!tab->select->cond)
      tab->select->cond= sort_table_cond;
    else
    {
      if (!(tab->select->cond=
	      new (thd->mem_root) Item_cond_and(thd,
                                                tab->select->cond,
                                                sort_table_cond)))
        DBUG_RETURN(true);
    }
    if (tab->pre_idx_push_select_cond)
    {
      if (sort_table_cond->type() == Item::COND_ITEM)
        sort_table_cond= sort_table_cond->copy_andor_structure(thd);
      if (!(tab->pre_idx_push_select_cond=
              new (thd->mem_root) Item_cond_and(thd,
                                                tab->pre_idx_push_select_cond,
                                                sort_table_cond)))
        DBUG_RETURN(true);
    }
    if (tab->select->cond && !tab->select->cond->fixed)
      tab->select->cond->fix_fields(thd, 0);
    if (tab->pre_idx_push_select_cond && !tab->pre_idx_push_select_cond->fixed)
      tab->pre_idx_push_select_cond->fix_fields(thd, 0);
    tab->select->pre_idx_push_select_cond= tab->pre_idx_push_select_cond;
    tab->set_select_cond(tab->select->cond, __LINE__);
    tab->select_cond->top_level_item();
    DBUG_EXECUTE("where",print_where(tab->select->cond,
				     "select and having",
                                     QT_ORDINARY););

    having= make_cond_for_table(thd, tmp_having, ~ (table_map) 0,
                                ~used_tables, false, false, false);
    DBUG_EXECUTE("where",
                 print_where(having, "having after sort", QT_ORDINARY););
  }

  DBUG_RETURN(false);
}