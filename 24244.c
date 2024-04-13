close_mysql_tables(THD *thd)
{
  if (! thd->in_sub_stmt)
    trans_commit_stmt(thd);
  close_thread_tables(thd);
  thd->release_transactional_locks();
}