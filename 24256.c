open_log_table(THD *thd, TABLE_LIST *one_table, Open_tables_backup *backup)
{
  uint flags= ( MYSQL_OPEN_IGNORE_GLOBAL_READ_LOCK |
                MYSQL_LOCK_IGNORE_GLOBAL_READ_ONLY |
                MYSQL_OPEN_IGNORE_FLUSH |
                MYSQL_LOCK_IGNORE_TIMEOUT |
                MYSQL_LOCK_LOG_TABLE);
  TABLE *table;
  /* Save value that is changed in mysql_lock_tables() */
  ulonglong save_utime_after_lock= thd->utime_after_lock;
  DBUG_ENTER("open_log_table");

  thd->reset_n_backup_open_tables_state(backup);

  if ((table= open_ltable(thd, one_table, one_table->lock_type, flags)))
  {
    DBUG_ASSERT(table->s->table_category == TABLE_CATEGORY_LOG);
    /* Make sure all columns get assigned to a default value */
    table->use_all_columns();
    DBUG_ASSERT(table->no_replicate);
  }
  else
    thd->restore_backup_open_tables_state(backup);

  thd->utime_after_lock= save_utime_after_lock;
  DBUG_RETURN(table);
}