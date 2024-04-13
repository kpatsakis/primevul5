static bool upgrade_lock_if_not_exists(THD *thd,
                                       const DDL_options_st &create_info,
                                       TABLE_LIST *create_table,
                                       ulong lock_wait_timeout)
{
  DBUG_ENTER("upgrade_lock_if_not_exists");

  if (thd->lex->sql_command == SQLCOM_CREATE_TABLE ||
      thd->lex->sql_command == SQLCOM_CREATE_SEQUENCE)
  {
    DEBUG_SYNC(thd,"create_table_before_check_if_exists");
    if (!create_info.or_replace() &&
        ha_table_exists(thd, &create_table->db, &create_table->table_name))
    {
      if (create_info.if_not_exists())
      {
        push_warning_printf(thd, Sql_condition::WARN_LEVEL_NOTE,
                            ER_TABLE_EXISTS_ERROR,
                            ER_THD(thd, ER_TABLE_EXISTS_ERROR),
                            create_table->table_name.str);
      }
      else
        my_error(ER_TABLE_EXISTS_ERROR, MYF(0), create_table->table_name.str);
      DBUG_RETURN(true);
    }
    DBUG_RETURN(thd->mdl_context.upgrade_shared_lock(
                                   create_table->mdl_request.ticket,
                                   MDL_EXCLUSIVE,
                                   lock_wait_timeout));
  }
  DBUG_RETURN(false);
}