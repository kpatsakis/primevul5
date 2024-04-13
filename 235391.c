TABLE *find_table_for_mdl_upgrade(THD *thd, const char *db,
                                  const char *table_name, int *p_error)
{
  TABLE *tab= find_locked_table(thd->open_tables, db, table_name);
  int error;

  if (unlikely(!tab))
  {
    error= ER_TABLE_NOT_LOCKED;
    goto err_exit;
  }

  /*
    It is not safe to upgrade the metadata lock without a global IX lock.
    This can happen with FLUSH TABLES <list> WITH READ LOCK as we in these
    cases don't take a global IX lock in order to be compatible with
    global read lock.
  */
  if (unlikely(!thd->mdl_context.is_lock_owner(MDL_key::BACKUP, "", "",
                                               MDL_BACKUP_DDL)))
  {
    error= ER_TABLE_NOT_LOCKED_FOR_WRITE;
    goto err_exit;
  }

  while (tab->mdl_ticket != NULL &&
         !tab->mdl_ticket->is_upgradable_or_exclusive() &&
         (tab= find_locked_table(tab->next, db, table_name)))
    continue;

  if (unlikely(!tab))
  {
    error= ER_TABLE_NOT_LOCKED_FOR_WRITE;
    goto err_exit;
  }

  return tab;

err_exit:
  if (p_error)
    *p_error= error;
  else
    my_error(error, MYF(0), table_name);

  return NULL;
}