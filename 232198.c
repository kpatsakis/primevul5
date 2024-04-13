enum Wsrep_SE_init_result wsrep_SE_init_wait()
{
  mysql_mutex_lock (&LOCK_wsrep_sst_init);
  while (SE_initialized == WSREP_SE_INIT_RESULT_NONE)
  {
    mysql_cond_wait (&COND_wsrep_sst_init, &LOCK_wsrep_sst_init);
  }
  enum Wsrep_SE_init_result ret= SE_initialized;
  mysql_mutex_unlock (&LOCK_wsrep_sst_init);
  return ret;
}