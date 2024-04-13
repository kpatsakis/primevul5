Locked_tables_list::unlock_locked_table(THD *thd, MDL_ticket *mdl_ticket)
{
  /*
    Ensure we are in locked table mode.
    As this function is only called on error condition it's better
    to check this condition here than in the caller.
  */
  if (thd->locked_tables_mode != LTM_LOCK_TABLES)
    return;

  if (mdl_ticket)
  {
    /*
      Under LOCK TABLES we may have several instances of table open
      and locked and therefore have to remove several metadata lock
      requests associated with them.
    */
    thd->mdl_context.release_all_locks_for_name(mdl_ticket);
  }

  if (thd->lock->table_count == 0)
    unlock_locked_tables(thd);
}