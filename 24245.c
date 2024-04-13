close_system_tables(THD *thd, Open_tables_backup *backup)
{
  close_thread_tables(thd);
  thd->restore_backup_open_tables_state(backup);
}