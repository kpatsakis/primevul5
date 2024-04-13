static void fix_prepare_info_in_table_list(THD *thd, TABLE_LIST *tbl)
{
  for (; tbl; tbl= tbl->next_local)
  {
    if (tbl->on_expr && !tbl->prep_on_expr)
    {
      thd->check_and_register_item_tree(&tbl->prep_on_expr, &tbl->on_expr);
      tbl->on_expr= tbl->on_expr->copy_andor_structure(thd);
    }
    if (tbl->is_view_or_derived() && tbl->is_merged_derived())
    {
      SELECT_LEX *sel= tbl->get_single_select();
      fix_prepare_info_in_table_list(thd, sel->get_table_list());
    }
  }
}