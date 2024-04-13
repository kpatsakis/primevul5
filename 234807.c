free_tmp_table(THD *thd, TABLE *entry)
{
  MEM_ROOT own_root= entry->mem_root;
  const char *save_proc_info;
  DBUG_ENTER("free_tmp_table");
  DBUG_PRINT("enter",("table: %s  alias: %s",entry->s->table_name.str,
                      entry->alias.c_ptr()));

  save_proc_info=thd->proc_info;
  THD_STAGE_INFO(thd, stage_removing_tmp_table);

  if (entry->file && entry->is_created())
  {
    entry->file->ha_index_or_rnd_end();
    if (entry->db_stat)
    {
      entry->file->info(HA_STATUS_VARIABLE);
      thd->tmp_tables_size+= (entry->file->stats.data_file_length +
                              entry->file->stats.index_file_length);
      entry->file->ha_drop_table(entry->s->path.str);
    }
    else
      entry->file->ha_delete_table(entry->s->path.str);
    delete entry->file;
  }

  /* free blobs */
  for (Field **ptr=entry->field ; *ptr ; ptr++)
    (*ptr)->free();

  if (entry->temp_pool_slot != MY_BIT_NONE)
    bitmap_lock_clear_bit(&temp_pool, entry->temp_pool_slot);

  plugin_unlock(0, entry->s->db_plugin);
  entry->alias.free();

  if (entry->pos_in_table_list && entry->pos_in_table_list->table)
  {
    DBUG_ASSERT(entry->pos_in_table_list->table == entry);
    entry->pos_in_table_list->table= NULL;
  }

  free_root(&own_root, MYF(0)); /* the table is allocated in its own root */
  thd_proc_info(thd, save_proc_info);

  DBUG_VOID_RETURN;
}