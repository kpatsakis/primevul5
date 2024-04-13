void JOIN_TAB::cleanup()
{
  DBUG_ENTER("JOIN_TAB::cleanup");
  
  DBUG_PRINT("enter", ("tab: %p  table %s.%s",
                       this,
                       (table ? table->s->db.str : "?"),
                       (table ? table->s->table_name.str : "?")));
  delete select;
  select= 0;
  delete quick;
  quick= 0;
  if (cache)
  {
    cache->free();
    cache= 0;
  }
  limit= 0;
  // Free select that was created for filesort outside of create_sort_index
  if (filesort && filesort->select && !filesort->own_select)
    delete filesort->select;
  delete filesort;
  filesort= NULL;
  /* Skip non-existing derived tables/views result tables */
  if (table &&
      (table->s->tmp_table != INTERNAL_TMP_TABLE || table->is_created()))
  {
    table->file->ha_end_keyread();
    table->file->ha_index_or_rnd_end();
  }
  if (table)
  {
    table->file->ha_end_keyread();
    table->file->ha_index_or_rnd_end();
    preread_init_done= FALSE;
    if (table->pos_in_table_list && 
        table->pos_in_table_list->jtbm_subselect)
    {
      if (table->pos_in_table_list->jtbm_subselect->is_jtbm_const_tab)
      {
        /*
          Set this to NULL so that cleanup_empty_jtbm_semi_joins() doesn't
          attempt to make another free_tmp_table call.
        */
        table->pos_in_table_list->table= NULL;
        free_tmp_table(join->thd, table);
        table= NULL;
      }
      else
      {
        TABLE_LIST *tmp= table->pos_in_table_list;
        end_read_record(&read_record);
        tmp->jtbm_subselect->cleanup();
        /* 
          The above call freed the materializedd temptable. Set it to NULL so
          that we don't attempt to touch it if JOIN_TAB::cleanup() is invoked
          multiple times (it may be)
        */
        tmp->table= NULL;
        table= NULL;
      }
      DBUG_VOID_RETURN;
    }
    /*
      We need to reset this for next select
      (Tested in part_of_refkey)
    */
    table->reginfo.join_tab= 0;
  }
  end_read_record(&read_record);
  explain_plan= NULL;
  DBUG_VOID_RETURN;
}