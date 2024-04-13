static void plugin_load(MEM_ROOT *tmp_root)
{
  TABLE_LIST tables;
  TABLE *table;
  READ_RECORD read_record_info;
  int error;
  THD *new_thd= new THD(0);
  bool result;
  DBUG_ENTER("plugin_load");

  if (global_system_variables.log_warnings >= 9)
    sql_print_information("Initializing installed plugins");

  new_thd->thread_stack= (char*) &tables;
  new_thd->store_globals();
  new_thd->db= my_strdup("mysql", MYF(0));
  new_thd->db_length= 5;
  bzero((char*) &new_thd->net, sizeof(new_thd->net));
  tables.init_one_table(STRING_WITH_LEN("mysql"), STRING_WITH_LEN("plugin"),
                        "plugin", TL_READ);
  tables.open_strategy= TABLE_LIST::OPEN_NORMAL;

  result= open_and_lock_tables(new_thd, &tables, FALSE, MYSQL_LOCK_IGNORE_TIMEOUT);

  table= tables.table;
  if (result)
  {
    DBUG_PRINT("error",("Can't open plugin table"));
    if (!opt_help)
      sql_print_error("Could not open mysql.plugin table. "
                      "Some plugins may be not loaded");
    else
      sql_print_warning("Could not open mysql.plugin table. "
                        "Some options may be missing from the help text");
    goto end;
  }

  if (init_read_record(&read_record_info, new_thd, table, NULL, NULL, 1, 0,
                       FALSE))
  {
    sql_print_error("Could not initialize init_read_record; Plugins not "
                    "loaded");
    goto end;
  }
  table->use_all_columns();
  while (!(error= read_record_info.read_record(&read_record_info)))
  {
    DBUG_PRINT("info", ("init plugin record"));
    String str_name, str_dl;
    get_field(tmp_root, table->field[0], &str_name);
    get_field(tmp_root, table->field[1], &str_dl);

    LEX_STRING name= {(char *)str_name.ptr(), str_name.length()};
    LEX_STRING dl= {(char *)str_dl.ptr(), str_dl.length()};

    if (!name.length || !dl.length)
      continue;

    /*
      there're no other threads running yet, so we don't need a mutex.
      but plugin_add() before is designed to work in multi-threaded
      environment, and it uses mysql_mutex_assert_owner(), so we lock
      the mutex here to satisfy the assert
    */
    mysql_mutex_lock(&LOCK_plugin);
    plugin_add(tmp_root, &name, &dl, REPORT_TO_LOG);
    free_root(tmp_root, MYF(MY_MARK_BLOCKS_FREE));
    mysql_mutex_unlock(&LOCK_plugin);
  }
  if (error > 0)
    sql_print_error(ER_THD(new_thd, ER_GET_ERRNO), my_errno,
                           table->file->table_type());
  end_read_record(&read_record_info);
  table->mark_table_for_reopen();
  close_mysql_tables(new_thd);
end:
  delete new_thd;
  DBUG_VOID_RETURN;
}