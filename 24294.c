bool restart_trans_for_tables(THD *thd, TABLE_LIST *table)
{
  DBUG_ENTER("restart_trans_for_tables");

  for (; table; table= table->next_global)
  {
    if (table->placeholder())
      continue;

    if (check_lock_and_start_stmt(thd, thd->lex, table))
    {
      DBUG_ASSERT(0);                           // Should never happen
      DBUG_RETURN(TRUE);
    }
  }
  DBUG_RETURN(FALSE);
}