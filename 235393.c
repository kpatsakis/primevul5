bool close_cached_connection_tables(THD *thd, LEX_CSTRING *connection)
{
  bool res= false;
  close_cached_connection_tables_arg argument;
  DBUG_ENTER("close_cached_connections");
  DBUG_ASSERT(thd);

  argument.thd= thd;
  argument.connection= connection;
  argument.tables= NULL;

  if (tdc_iterate(thd,
                  (my_hash_walk_action) close_cached_connection_tables_callback,
                  &argument))
    DBUG_RETURN(true);

  for (TABLE_LIST *table= argument.tables; table; table= table->next_local)
    res|= tdc_remove_table(thd, TDC_RT_REMOVE_UNUSED,
                           table->db.str,
                           table->table_name.str, TRUE);

  /* Return true if we found any open connections */
  DBUG_RETURN(res);
}