JOIN_TAB::remove_duplicates()

{
  bool error;
  ulong keylength= 0;
  uint field_count;
  List<Item> *fields= (this-1)->fields;
  THD *thd= join->thd;

  DBUG_ENTER("remove_duplicates");

  DBUG_ASSERT(join->aggr_tables > 0 && table->s->tmp_table != NO_TMP_TABLE);
  THD_STAGE_INFO(join->thd, stage_removing_duplicates);

  //join->explain->ops_tracker.report_duplicate_removal();

  table->reginfo.lock_type=TL_WRITE;

  /* Calculate how many saved fields there is in list */
  field_count=0;
  List_iterator<Item> it(*fields);
  Item *item;
  while ((item=it++))
  {
    if (item->get_tmp_table_field() && ! item->const_item())
      field_count++;
  }

  if (!field_count && !(join->select_options & OPTION_FOUND_ROWS) && !having) 
  {                    // only const items with no OPTION_FOUND_ROWS
    join->unit->select_limit_cnt= 1;		// Only send first row
    DBUG_RETURN(false);
  }

  Field **first_field=table->field+table->s->fields - field_count;
  for (Field **ptr=first_field; *ptr; ptr++)
    keylength+= (*ptr)->sort_length() + (*ptr)->maybe_null();

  /*
    Disable LIMIT ROWS EXAMINED in order to avoid interrupting prematurely
    duplicate removal, and produce a possibly incomplete query result.
  */
  thd->lex->limit_rows_examined_cnt= ULONGLONG_MAX;
  if (thd->killed == ABORT_QUERY)
    thd->reset_killed();

  table->file->info(HA_STATUS_VARIABLE);
  if (table->s->db_type() == heap_hton ||
      (!table->s->blob_fields &&
       ((ALIGN_SIZE(keylength) + HASH_OVERHEAD) * table->file->stats.records <
	thd->variables.sortbuff_size)))
    error=remove_dup_with_hash_index(join->thd, table, field_count, first_field,
				     keylength, having);
  else
    error=remove_dup_with_compare(join->thd, table, first_field, having);

  if (join->select_lex != join->select_lex->master_unit()->fake_select_lex)
    thd->lex->set_limit_rows_examined();
  free_blobs(first_field);
  DBUG_RETURN(error);
}