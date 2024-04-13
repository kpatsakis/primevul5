make_cond_for_table(THD *thd, Item *cond, table_map tables,
                    table_map used_table,
                    int join_tab_idx_arg,
                    bool exclude_expensive_cond __attribute__((unused)),
		    bool retain_ref_cond)
{
  return make_cond_for_table_from_pred(thd, cond, cond, tables, used_table,
                                       join_tab_idx_arg,
                                       exclude_expensive_cond,
                                       retain_ref_cond, true);
}