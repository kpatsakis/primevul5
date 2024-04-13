void drop_open_table(THD *thd, TABLE *table, const char *db_name,
                     const char *table_name)
{
  DBUG_ENTER("drop_open_table");
  if (table->s->tmp_table)
    thd->drop_temporary_table(table, NULL, true);
  else
  {
    DBUG_ASSERT(table == thd->open_tables);

    handlerton *table_type= table->s->db_type();
    table->file->extra(HA_EXTRA_PREPARE_FOR_DROP);
    close_thread_table(thd, &thd->open_tables);
    /* Remove the table share from the table cache. */
    tdc_remove_table(thd, TDC_RT_REMOVE_ALL, db_name, table_name,
                     FALSE);
    /* Remove the table from the storage engine and rm the .frm. */
    quick_rm_table(thd, table_type, db_name, table_name, 0);
 }
  DBUG_VOID_RETURN;
}