thr_lock_type read_lock_type_for_table(THD *thd,
                                       Query_tables_list *prelocking_ctx,
                                       TABLE_LIST *table_list,
                                       bool routine_modifies_data)
{
  /*
    In cases when this function is called for a sub-statement executed in
    prelocked mode we can't rely on OPTION_BIN_LOG flag in THD::options
    bitmap to determine that binary logging is turned on as this bit can
    be cleared before executing sub-statement. So instead we have to look
    at THD::variables::sql_log_bin member.
  */
  bool log_on= mysql_bin_log.is_open() && thd->variables.sql_log_bin;
  if ((log_on == FALSE) || (thd->wsrep_binlog_format() == BINLOG_FORMAT_ROW) ||
      (table_list->table->s->table_category == TABLE_CATEGORY_LOG) ||
      (table_list->table->s->table_category == TABLE_CATEGORY_PERFORMANCE) ||
      !(is_update_query(prelocking_ctx->sql_command) ||
        (routine_modifies_data && table_list->prelocking_placeholder) ||
        (thd->locked_tables_mode > LTM_LOCK_TABLES)))
    return TL_READ;
  else
    return TL_READ_NO_INSERT;
}