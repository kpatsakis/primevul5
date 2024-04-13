void st_select_lex::fix_prepare_information(THD *thd, Item **conds, 
                                            Item **having_conds)
{
  DBUG_ENTER("st_select_lex::fix_prepare_information");
  if (!thd->stmt_arena->is_conventional() &&
      !(changed_elements & TOUCHED_SEL_COND))
  {
    changed_elements|= TOUCHED_SEL_COND;
    if (group_list.first)
    {
      if (!group_list_ptrs)
      {
        void *mem= thd->stmt_arena->alloc(sizeof(Group_list_ptrs));
        group_list_ptrs= new (mem) Group_list_ptrs(thd->stmt_arena->mem_root);
      }
      group_list_ptrs->reserve(group_list.elements);
      for (ORDER *order= group_list.first; order; order= order->next)
      {
        group_list_ptrs->push_back(order);
      }
    }
    if (*conds)
    {
      thd->check_and_register_item_tree(&prep_where, conds);
      *conds= where= prep_where->copy_andor_structure(thd);
    }
    if (*having_conds)
    {
      thd->check_and_register_item_tree(&prep_having, having_conds);
      *having_conds= having= prep_having->copy_andor_structure(thd);
    }
    fix_prepare_info_in_table_list(thd, table_list.first);
  }
  DBUG_VOID_RETURN;
}