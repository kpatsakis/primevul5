OPEN_TABLE_LIST *list_open_tables(THD *thd, const char *db, const char *wild)
{
  list_open_tables_arg argument;
  DBUG_ENTER("list_open_tables");

  argument.thd= thd;
  argument.db= db;
  argument.wild= wild;
  bzero((char*) &argument.table_list, sizeof(argument.table_list));
  argument.start_list= &argument.open_list;
  argument.open_list= 0;

  if (tdc_iterate(thd, (my_hash_walk_action) list_open_tables_callback,
                  &argument, true))
    DBUG_RETURN(0);

  DBUG_RETURN(argument.open_list);
}