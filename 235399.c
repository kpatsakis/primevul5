void purge_tables(bool purge_flag)
{
  /*
    Force close of all open tables.

    Note that code in TABLE_SHARE::wait_for_old_version() assumes that
    incrementing of refresh_version is followed by purge of unused table
    shares.
  */
  kill_delayed_threads();
  /*
    Get rid of all unused TABLE and TABLE_SHARE instances. By doing
    this we automatically close all tables which were marked as "old".
  */
  tc_purge(purge_flag);
  /* Free table shares which were not freed implicitly by loop above. */
  tdc_purge(true);
}