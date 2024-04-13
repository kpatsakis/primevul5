create_sort_index(THD *thd, JOIN *join, JOIN_TAB *tab, Filesort *fsort)
{
  TABLE *table;
  SQL_SELECT *select;
  bool quick_created= FALSE;
  SORT_INFO *file_sort= 0;
  DBUG_ENTER("create_sort_index");

  if (fsort == NULL)
    fsort= tab->filesort;

  table=  tab->table;
  select= fsort->select;
 
  table->status=0;				// May be wrong if quick_select

  if (!tab->preread_init_done && tab->preread_init())
    goto err;

  // If table has a range, move it to select
  if (select && tab->ref.key >= 0)
  {
    if (!select->quick)
    {
      if (tab->quick)
      {
        select->quick= tab->quick;
        tab->quick= NULL;
      /* 
        We can only use 'Only index' if quick key is same as ref_key
        and in index_merge 'Only index' cannot be used
      */
      if (((uint) tab->ref.key != select->quick->index))
        table->file->ha_end_keyread();
      }
      else
      {
        /*
	  We have a ref on a const;  Change this to a range that filesort
	  can use.
	  For impossible ranges (like when doing a lookup on NULL on a NOT NULL
	  field, quick will contain an empty record set.
        */
        if (!(select->quick= (tab->type == JT_FT ?
			      get_ft_select(thd, table, tab->ref.key) :
			      get_quick_select_for_ref(thd, table, &tab->ref, 
                                                       tab->found_records))))
	  goto err;
        quick_created= TRUE;
      }
      fsort->own_select= true;
    }
    else
    {
      DBUG_ASSERT(tab->type == JT_REF || tab->type == JT_EQ_REF);
      // Update ref value
      if (unlikely(cp_buffer_from_ref(thd, table, &tab->ref) &&
                   thd->is_fatal_error))
        goto err;                                   // out of memory
    }
  }

 
  /* Fill schema tables with data before filesort if it's necessary */
  if ((join->select_lex->options & OPTION_SCHEMA_TABLE) &&
      unlikely(get_schema_tables_result(join, PROCESSED_BY_CREATE_SORT_INDEX)))
    goto err;

  if (table->s->tmp_table)
    table->file->info(HA_STATUS_VARIABLE);	// Get record count
  file_sort= filesort(thd, table, fsort, fsort->tracker, join, tab->table->map);
  DBUG_ASSERT(tab->filesort_result == 0);
  tab->filesort_result= file_sort;
  tab->records= 0;
  if (file_sort)
  {
    tab->records= join->select_options & OPTION_FOUND_ROWS ?
      file_sort->found_rows : file_sort->return_rows;
    tab->join->join_examined_rows+= file_sort->examined_rows;
  }

  if (quick_created)
  {
    /* This will delete the quick select. */
    select->cleanup();
  }
 
  table->file->ha_end_keyread();
  if (tab->type == JT_FT)
    table->file->ha_ft_end();
  else
    table->file->ha_index_or_rnd_end();

  DBUG_RETURN(file_sort == 0);
err:
  DBUG_RETURN(-1);
}