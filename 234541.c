bool build_tmp_join_prefix_cond(JOIN *join, JOIN_TAB *last_tab, Item **ret)
{
  THD *const thd= join->thd;
  Item_cond_and *all_conds= NULL;

  Item *res= NULL;

  // Pick the ON-expression. Use the same logic as in get_sargable_cond():
  if (last_tab->on_expr_ref)
    res= *last_tab->on_expr_ref;
  else if (last_tab->table->pos_in_table_list &&
           last_tab->table->pos_in_table_list->embedding &&
           !last_tab->table->pos_in_table_list->embedding->sj_on_expr)
  {
    res= last_tab->table->pos_in_table_list->embedding->on_expr;
  }

  for (JOIN_TAB *tab= first_depth_first_tab(join);
       tab;
       tab= next_depth_first_tab(join, tab))
  {
    if (tab->select_cond)
    {
      if (!res)
        res= tab->select_cond;
      else
      {
        if (!all_conds)
        {
          if (!(all_conds= new (thd->mem_root)Item_cond_and(thd, res,
                                                            tab->select_cond)))
            return true;
          res= all_conds;
        }
        else
          all_conds->add(tab->select_cond, thd->mem_root);
      }
    }
    if (tab == last_tab)
      break;
  }
  *ret= all_conds? all_conds: res;
  return false;
}