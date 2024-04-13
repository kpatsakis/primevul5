close_all_tables_for_name(THD *thd, TABLE_SHARE *share,
                          ha_extra_function extra,
                          TABLE *skip_table)
{
  DBUG_ASSERT(!share->tmp_table);

  char key[MAX_DBKEY_LENGTH];
  uint key_length= share->table_cache_key.length;
  const char *db= key;
  const char *table_name= db + share->db.length + 1;
  bool remove_from_locked_tables= extra != HA_EXTRA_NOT_USED;

  memcpy(key, share->table_cache_key.str, key_length);

  for (TABLE **prev= &thd->open_tables; *prev; )
  {
    TABLE *table= *prev;

    if (table->s->table_cache_key.length == key_length &&
        !memcmp(table->s->table_cache_key.str, key, key_length) &&
        table != skip_table)
    {
      thd->locked_tables_list.unlink_from_list(thd,
                                               table->pos_in_locked_tables,
                                               remove_from_locked_tables);
      /* Inform handler that there is a drop table or a rename going on */
      if (extra != HA_EXTRA_NOT_USED && table->db_stat)
      {
        table->file->extra(extra);
        extra= HA_EXTRA_NOT_USED;               // Call extra once!
      }

      /*
        Does nothing if the table is not locked.
        This allows one to use this function after a table
        has been unlocked, e.g. in partition management.
      */
      mysql_lock_remove(thd, thd->lock, table);
      close_thread_table(thd, prev);
    }
    else
    {
      /* Step to next entry in open_tables list. */
      prev= &table->next;
    }
  }
  if (skip_table == NULL)
  {
    /* Remove the table share from the cache. */
    tdc_remove_table(thd, TDC_RT_REMOVE_ALL, db, table_name,
                     FALSE);
  }
}