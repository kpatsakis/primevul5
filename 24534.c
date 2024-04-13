int ha_maria::check_for_upgrade(HA_CHECK_OPT *check)
{
  if (table->s->mysql_version && table->s->mysql_version <= 100509 &&
      (file->s->base.extra_options & MA_EXTRA_OPTIONS_ENCRYPTED))
  {
    /*
      Encrypted tables before 10.5.9 had a bug where LSN was not
      stored on the pages. These must be repaired!
    */
    return HA_ADMIN_NEEDS_ALTER;
  }
  return HA_ADMIN_OK;
}