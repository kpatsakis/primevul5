static bool multi_update_check_table_access(THD *thd, TABLE_LIST *table,
                                            table_map tables_for_update,
                                            bool *updated_arg)
{
  if (table->view)
  {
    bool updated= false;
    /*
      If it is a mergeable view then we need to check privileges on its
      underlying tables being merged (including views). We also need to
      check if any of them is updated in order to find if this view is
      updated.
      If it is a non-mergeable view then it can't be updated.
    */
    DBUG_ASSERT(table->merge_underlying_list ||
                (!table->updatable &&
                 !(table->table->map & tables_for_update)));

    for (TABLE_LIST *tbl= table->merge_underlying_list; tbl;
         tbl= tbl->next_local)
    {
      if (multi_update_check_table_access(thd, tbl, tables_for_update,
                                          &updated))
      {
        tbl->hide_view_error(thd);
        return true;
      }
    }
    if (check_table_access(thd, updated ? UPDATE_ACL: SELECT_ACL, table,
                           FALSE, 1, FALSE))
      return true;
    *updated_arg|= updated;
    /* We only need SELECT privilege for columns in the values list. */
    table->grant.want_privilege= SELECT_ACL & ~table->grant.privilege;
  }
  else
  {
    /* Must be a base or derived table. */
    const bool updated= table->table->map & tables_for_update;
    if (check_table_access(thd, updated ? UPDATE_ACL : SELECT_ACL, table,
                           FALSE, 1, FALSE))
      return true;
    *updated_arg|= updated;
    /* We only need SELECT privilege for columns in the values list. */
    if (!table->derived)
    {
      table->grant.want_privilege= SELECT_ACL & ~table->grant.privilege;
      table->table->grant.want_privilege= (SELECT_ACL &
                                           ~table->table->grant.privilege);
    }
  }
  return false;
}