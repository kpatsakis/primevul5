bool check_unique_table(THD *thd, TABLE_LIST *table_list)
{
  TABLE_LIST *duplicate;
  DBUG_ENTER("check_unique_table");
  if ((duplicate= unique_table(thd, table_list, table_list->next_global, 0)))
  {
    update_non_unique_table_error(table_list, "UPDATE", duplicate);
    DBUG_RETURN(TRUE);
  }
  DBUG_RETURN(FALSE);
}