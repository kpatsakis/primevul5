static int generate_binlog_opt_val(char** ret)
{
  DBUG_ASSERT(ret);
  *ret= NULL;
  if (opt_bin_log && gtid_mode > 0)
  {
    assert(opt_bin_logname);
    *ret= my_strdup(opt_bin_logname, MYF(0));
  }
  else
  {
    *ret= my_strdup("", MYF(0));
  }
  if (!*ret) return -ENOMEM;
  return 0;
}