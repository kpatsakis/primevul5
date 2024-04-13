static bool add_ref_to_table_cond(THD *thd, JOIN_TAB *join_tab)
{
  DBUG_ENTER("add_ref_to_table_cond");
  if (!join_tab->ref.key_parts)
    DBUG_RETURN(FALSE);

  Item_cond_and *cond= new (thd->mem_root) Item_cond_and(thd);
  TABLE *table=join_tab->table;
  int error= 0;
  if (!cond)
    DBUG_RETURN(TRUE);

  for (uint i=0 ; i < join_tab->ref.key_parts ; i++)
  {
    Field *field=table->field[table->key_info[join_tab->ref.key].key_part[i].
			      fieldnr-1];
    Item *value=join_tab->ref.items[i];
    cond->add(new (thd->mem_root)
              Item_func_equal(thd, new (thd->mem_root) Item_field(thd, field),
                              value),
              thd->mem_root);
  }
  if (thd->is_fatal_error)
    DBUG_RETURN(TRUE);
  if (!cond->fixed)
  {
    Item *tmp_item= (Item*) cond;
    cond->fix_fields(thd, &tmp_item);
    DBUG_ASSERT(cond == tmp_item);
  }
  if (join_tab->select)
  {
    Item *UNINIT_VAR(cond_copy);
    if (join_tab->select->pre_idx_push_select_cond)
      cond_copy= cond->copy_andor_structure(thd);
    if (join_tab->select->cond)
      error=(int) cond->add(join_tab->select->cond, thd->mem_root);
    join_tab->select->cond= cond;
    if (join_tab->select->pre_idx_push_select_cond)
    {
      Item *new_cond= and_conds(thd, cond_copy,
                                join_tab->select->pre_idx_push_select_cond);
      if (!new_cond->fixed && new_cond->fix_fields(thd, &new_cond))
        error= 1;
      join_tab->pre_idx_push_select_cond=
        join_tab->select->pre_idx_push_select_cond= new_cond;
    }
    join_tab->set_select_cond(cond, __LINE__);
  }
  else if ((join_tab->select= make_select(join_tab->table, 0, 0, cond,
                                          (SORT_INFO*) 0, 0, &error)))
    join_tab->set_select_cond(cond, __LINE__);

  DBUG_RETURN(error ? TRUE : FALSE);
}