void dbug_serve_apcs(THD *thd, int n_calls)
{
  const char *save_proc_info= thd->proc_info;
  
  /* Busy-wait for n_calls APC requests to arrive and be processed */
  int n_apcs= thd->apc_target.n_calls_processed + n_calls;
  while (thd->apc_target.n_calls_processed < n_apcs)
  {
    /* This is so that mysqltest knows we're ready to serve requests: */
    thd_proc_info(thd, "show_explain_trap");
    my_sleep(30000);
    thd_proc_info(thd, save_proc_info);
    if (thd->check_killed())
      break;
  }
}