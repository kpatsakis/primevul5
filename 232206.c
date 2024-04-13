void wsrep_SE_initialized(enum Wsrep_SE_init_result result)
{
  mysql_mutex_lock (&LOCK_wsrep_sst_init);
  if (SE_initialized == WSREP_SE_INIT_RESULT_NONE)
  {
    SE_initialized= result;
  }
  mysql_cond_signal (&COND_wsrep_sst_init);
  mysql_mutex_unlock (&LOCK_wsrep_sst_init);
}