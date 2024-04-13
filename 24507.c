void maria_prepare_for_backup()
{
  translog_disable_purge();
}