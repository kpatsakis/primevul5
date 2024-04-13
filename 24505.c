bool ha_maria::auto_repair(int error) const
{
  /* Always auto-repair moved tables (error == HA_ERR_OLD_FILE) */
  return ((MY_TEST(maria_recover_options & HA_RECOVER_ANY) &&
           error == HA_ERR_CRASHED_ON_USAGE) ||
          error == HA_ERR_OLD_FILE);

}