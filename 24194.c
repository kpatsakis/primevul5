void close_log_table(THD *thd, Open_tables_backup *backup)
{
  close_system_tables(thd, backup);
}