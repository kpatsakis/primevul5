static bool auto_repair_table(THD *thd, TABLE_LIST *table_list)
{
  TABLE_SHARE *share;
  TABLE *entry;
  bool result= TRUE;

  thd->clear_error();

  if (!(entry= (TABLE*)my_malloc(sizeof(TABLE), MYF(MY_WME))))
    return result;

  if (!(share= tdc_acquire_share(thd, table_list, GTS_TABLE)))
    goto end_free;

  DBUG_ASSERT(! share->is_view);

  if (open_table_from_share(thd, share, table_list->alias,
                            HA_OPEN_KEYFILE | HA_TRY_READ_ONLY,
                            EXTRA_RECORD,
                            ha_open_options | HA_OPEN_FOR_REPAIR,
                            entry, FALSE) || ! entry->file ||
      (entry->file->is_crashed() && entry->file->ha_check_and_repair(thd)))
  {
    /* Give right error message */
    thd->clear_error();
    my_error(ER_NOT_KEYFILE, MYF(0), share->table_name.str);
    sql_print_error("Couldn't repair table: %s.%s", share->db.str,
                    share->table_name.str);
    if (entry->file)
      closefrm(entry);
  }
  else
  {
    thd->clear_error();			// Clear error message
    closefrm(entry);
    result= FALSE;
  }

  tdc_release_share(share);
  /* Remove the repaired share from the table cache. */
  tdc_remove_table(thd, TDC_RT_REMOVE_ALL,
                   table_list->db, table_list->table_name,
                   FALSE);
end_free:
  my_free(entry);
  return result;
}