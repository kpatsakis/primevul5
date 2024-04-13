static int sst_donate_mysqldump (const char*         addr,
                                 const wsrep_uuid_t* uuid,
                                 const char*         uuid_str,
                                 wsrep_seqno_t       seqno,
                                 bool                bypass,
                                 char**              env) // carries auth info
{
  int const cmd_len= 4096;
  wsp::string  cmd_str(cmd_len);

  if (!cmd_str())
  {
    WSREP_ERROR("sst_donate_mysqldump(): "
                "could not allocate cmd buffer of %d bytes", cmd_len);
    return -ENOMEM;
  }

  if (!bypass && wsrep_sst_donor_rejects_queries) sst_reject_queries(TRUE);

  int ret= snprintf (cmd_str(), cmd_len,
                     "wsrep_sst_mysqldump "
                     WSREP_SST_OPT_ADDR" '%s' "
                     WSREP_SST_OPT_LPORT" '%u' "
                     WSREP_SST_OPT_SOCKET" '%s' "
                     WSREP_SST_OPT_CONF" '%s' "
                     WSREP_SST_OPT_GTID" '%s:%lld'"
                     "%s",
                     addr, mysqld_port, mysqld_unix_port,
                     wsrep_defaults_file, uuid_str,
                     (long long)seqno, bypass ? " " WSREP_SST_OPT_BYPASS : "");

  if (ret < 0 || ret >= cmd_len)
  {
    WSREP_ERROR("sst_donate_mysqldump(): snprintf() failed: %d", ret);
    return (ret < 0 ? ret : -EMSGSIZE);
  }

  WSREP_DEBUG("Running: '%s'", cmd_str());

  ret= sst_run_shell (cmd_str(), env, 3);

  wsrep_gtid_t const state_id = { *uuid, (ret ? WSREP_SEQNO_UNDEFINED : seqno)};

  wsrep->sst_sent (wsrep, &state_id, ret);

  return ret;
}