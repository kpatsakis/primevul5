check_and_update_routine_version(THD *thd, Sroutine_hash_entry *rt,
                                 sp_head *sp)
{
  ulong spc_version= sp_cache_version();
  /* sp is NULL if there is no such routine. */
  ulong version= sp ? sp->sp_cache_version() : spc_version;
  /*
    If the version in the parse tree is stale,
    or the version in the cache is stale and sp is not used,
    we need to reprepare.
    Sic: version != spc_version <--> sp is not NULL.
  */
  if (rt->m_sp_cache_version != version ||
      (version != spc_version && !sp->is_invoked()))
  {
    if (thd->m_reprepare_observer &&
        thd->m_reprepare_observer->report_error(thd))
    {
      /*
        Version of the sp cache is different from the
        previous execution of the prepared statement, and it is
        unacceptable for this SQLCOM. Error has been reported.
      */
      DBUG_ASSERT(thd->is_error());
      return TRUE;
    }
    /* Always maintain the latest cache version. */
    rt->m_sp_cache_version= version;
  }
  return FALSE;
}