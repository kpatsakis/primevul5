bool maria_flush_logs(handlerton *hton)
{
  return MY_TEST(translog_purge_at_flush());
}