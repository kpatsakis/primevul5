bool init_read_record(READ_RECORD *info,THD *thd, TABLE *table,
		      SQL_SELECT *select,
                      SORT_INFO *filesort,
		      int use_record_cache, bool print_error, 
                      bool disable_rr_cache)
{
  IO_CACHE *tempfile;
  SORT_ADDON_FIELD *addon_field= filesort ? filesort->addon_field : 0;
  DBUG_ENTER("init_read_record");

  bzero((char*) info,sizeof(*info));
  info->thd=thd;
  info->table=table;
  info->forms= &info->table;		/* Only one table */
  info->addon_field= addon_field;
  
  if ((table->s->tmp_table == INTERNAL_TMP_TABLE) &&
      !addon_field)
    (void) table->file->extra(HA_EXTRA_MMAP);
  
  if (addon_field)
  {
    info->rec_buf=    (uchar*) filesort->addon_buf.str;
    info->ref_length= filesort->addon_buf.length;
    info->unpack=     filesort->unpack;
  }
  else
  {
    empty_record(table);
    info->record= table->record[0];
    info->ref_length= table->file->ref_length;
  }
  info->select=select;
  info->print_error=print_error;
  info->unlock_row= rr_unlock_row;
  info->ignore_not_found_rows= 0;
  table->status= 0;			/* Rows are always found */

  tempfile= 0;
  if (select && my_b_inited(&select->file))
    tempfile= &select->file;
  else if (filesort && my_b_inited(&filesort->io_cache))
    tempfile= &filesort->io_cache;

  if (tempfile && !(select && select->quick))
  {
    DBUG_PRINT("info",("using rr_from_tempfile"));
    info->read_record= (addon_field ?
                        rr_unpack_from_tempfile : rr_from_tempfile);
    info->io_cache= tempfile;
    reinit_io_cache(info->io_cache,READ_CACHE,0L,0,0);
    info->ref_pos=table->file->ref;
    if (!table->file->inited)
      if (table->file->ha_rnd_init_with_error(0))
        DBUG_RETURN(1);

    /*
      addon_field is checked because if we use addon fields,
      it doesn't make sense to use cache - we don't read from the table
      and filesort->io_cache is read sequentially
    */
    if (!disable_rr_cache &&
        !addon_field &&
	thd->variables.read_rnd_buff_size &&
	!(table->file->ha_table_flags() & HA_FAST_KEY_READ) &&
	(table->db_stat & HA_READ_ONLY ||
	 table->reginfo.lock_type <= TL_READ_NO_INSERT) &&
	(ulonglong) table->s->reclength* (table->file->stats.records+
                                          table->file->stats.deleted) >
	(ulonglong) MIN_FILE_LENGTH_TO_USE_ROW_CACHE &&
	info->io_cache->end_of_file/info->ref_length * table->s->reclength >
	(my_off_t) MIN_ROWS_TO_USE_TABLE_CACHE &&
	!table->s->blob_fields &&
        info->ref_length <= MAX_REFLENGTH)
    {
      if (! init_rr_cache(thd, info))
      {
	DBUG_PRINT("info",("using rr_from_cache"));
	info->read_record=rr_from_cache;
      }
    }
  }
  else if (select && select->quick)
  {
    DBUG_PRINT("info",("using rr_quick"));
    info->read_record=rr_quick;
  }
  else if (filesort && filesort->record_pointers)
  {
    DBUG_PRINT("info",("using record_pointers"));
    if (table->file->ha_rnd_init_with_error(0))
      DBUG_RETURN(1);
    info->cache_pos= filesort->record_pointers;
    info->cache_end= (info->cache_pos+ 
                      filesort->return_rows * info->ref_length);
    info->read_record= (addon_field ?
                        rr_unpack_from_buffer : rr_from_pointers);
  }
  else if (table->file->keyread_enabled())
  {
    int error;
    info->read_record= rr_index_first;
    if (!table->file->inited &&
        (error= table->file->ha_index_init(table->file->keyread, 1)))
    {
      if (print_error)
        table->file->print_error(error, MYF(0));
      DBUG_RETURN(1);
    }
  }
  else
  {
    DBUG_PRINT("info",("using rr_sequential"));
    info->read_record=rr_sequential;
    if (table->file->ha_rnd_init_with_error(1))
      DBUG_RETURN(1);
    /* We can use record cache if we don't update dynamic length tables */
    if (!table->no_cache &&
	(use_record_cache > 0 ||
	 (int) table->reginfo.lock_type <= (int) TL_READ_HIGH_PRIORITY ||
	 !(table->s->db_options_in_use & HA_OPTION_PACK_RECORD) ||
	 (use_record_cache < 0 &&
	  !(table->file->ha_table_flags() & HA_NOT_DELETE_WITH_CACHE))))
      (void) table->file->extra_opt(HA_EXTRA_CACHE,
                                    thd->variables.read_buff_size);
  }
  /* Condition pushdown to storage engine */
  if ((table->file->ha_table_flags() & HA_CAN_TABLE_CONDITION_PUSHDOWN) &&
      select && select->cond && 
      (select->cond->used_tables() & table->map) &&
      !table->file->pushed_cond)
    table->file->cond_push(select->cond);

  DBUG_RETURN(0);
} /* init_read_record */