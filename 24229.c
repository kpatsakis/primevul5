open_tables_check_upgradable_mdl(THD *thd, TABLE_LIST *tables_start,
                                 TABLE_LIST *tables_end, uint flags)
{
  TABLE_LIST *table;

  DBUG_ASSERT(thd->locked_tables_mode);

  for (table= tables_start; table && table != tables_end;
       table= table->next_global)
  {
    /*
      Check below needs to be updated if this function starts
      called for SRO locks.
    */
    DBUG_ASSERT(table->mdl_request.type != MDL_SHARED_READ_ONLY);
    if (table->mdl_request.type < MDL_SHARED_UPGRADABLE ||
        table->open_type == OT_TEMPORARY_ONLY ||
        (table->open_type == OT_TEMPORARY_OR_BASE && is_temporary_table(table)))
    {
      continue;
    }

    /*
      We don't need to do anything about the found TABLE instance as it
      will be handled later in open_tables(), we only need to check that
      an upgradable lock is already acquired. When we enter LOCK TABLES
      mode, SNRW locks are acquired before all other locks. So if under
      LOCK TABLES we find that there is TABLE instance with upgradeable
      lock, all other instances of TABLE for the same table will have the
      same ticket.

      Note that this works OK even for CREATE TABLE statements which
      request X type of metadata lock. This is because under LOCK TABLES
      such statements don't create the table but only check if it exists
      or, in most complex case, only insert into it.
      Thus SNRW lock should be enough.

      Note that find_table_for_mdl_upgrade() will report an error if
      no suitable ticket is found.
    */
    if (!find_table_for_mdl_upgrade(thd, table->db, table->table_name, NULL))
      return TRUE;
  }

  return FALSE;
}