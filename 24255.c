bool close_cached_connection_tables(THD *thd, LEX_STRING *connection)
{
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

  DBUG_RETURN(argument.tables ?
              close_cached_tables(thd, argument.tables, FALSE, LONG_TIMEOUT) :
              false);
}