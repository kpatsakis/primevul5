static int sst_donate_other (const char*   method,
                             const char*   addr,
                             const char*   uuid,
                             wsrep_seqno_t seqno,
                             bool          bypass,
                             char**        env) // carries auth info
{
  int const cmd_len= 4096;
  wsp::string  cmd_str(cmd_len);

  if (!cmd_str())
  {
    WSREP_ERROR("sst_donate_other(): "
                "could not allocate cmd buffer of %d bytes", cmd_len);
    return -ENOMEM;
  }

  const char* binlog_opt= "";
  char* binlog_opt_val= NULL;

  int ret;
  if ((ret= generate_binlog_opt_val(&binlog_opt_val)))
  {
    WSREP_ERROR("sst_donate_other(): generate_binlog_opt_val() failed: %d",ret);
    return ret;
  }
  if (strlen(binlog_opt_val)) binlog_opt= WSREP_SST_OPT_BINLOG;

  ret= snprintf (cmd_str(), cmd_len,
                 "wsrep_sst_%s "
                 WSREP_SST_OPT_ROLE" 'donor' "
                 WSREP_SST_OPT_ADDR" '%s' "
                 WSREP_SST_OPT_SOCKET" '%s' "
                 WSREP_SST_OPT_DATA" '%s' "
                 WSREP_SST_OPT_CONF" '%s' "
                 WSREP_SST_OPT_CONF_SUFFIX" '%s' "
                 " %s '%s' "
                 WSREP_SST_OPT_GTID" '%s:%lld'"
                 "%s",
                 method, addr, mysqld_unix_port, mysql_real_data_home,
                 wsrep_defaults_file, wsrep_defaults_group_suffix,
                 binlog_opt, binlog_opt_val,
                 uuid, (long long) seqno,
                 bypass ? " " WSREP_SST_OPT_BYPASS : "");
  my_free(binlog_opt_val);

  if (ret < 0 || ret >= cmd_len)
  {
    WSREP_ERROR("sst_donate_other(): snprintf() failed: %d", ret);
    return (ret < 0 ? ret : -EMSGSIZE);
  }

  if (!bypass && wsrep_sst_donor_rejects_queries) sst_reject_queries(FALSE);

  pthread_t tmp;
  sst_thread_arg arg(cmd_str(), env);
  mysql_mutex_lock (&arg.lock);
  ret = pthread_create (&tmp, NULL, sst_donor_thread, &arg);
  if (ret)
  {
    WSREP_ERROR("sst_donate_other(): pthread_create() failed: %d (%s)",
                ret, strerror(ret));
    return ret;
  }
  mysql_cond_wait (&arg.cond, &arg.lock);

  WSREP_INFO("sst_donor_thread signaled with %d", arg.err);
  return arg.err;
}