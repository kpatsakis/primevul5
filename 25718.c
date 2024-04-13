int multi_update::do_updates()
{
  TABLE_LIST *cur_table;
  int local_error= 0;
  ha_rows org_updated;
  TABLE *table, *tmp_table, *err_table;
  List_iterator_fast<TABLE> check_opt_it(unupdated_check_opt_tables);
  DBUG_ENTER("multi_update::do_updates");

  do_update= 0;					// Don't retry this function
  if (!found)
    DBUG_RETURN(0);

  /*
    Update read_set to include all fields that virtual columns may depend on.
    Usually they're already in the read_set, but if the previous access
    method was keyread, only the virtual column itself will be in read_set,
    not its dependencies
  */
  while(TABLE *tbl= check_opt_it++)
  {
    if (tbl->vcol_set)
    {
      bitmap_clear_all(tbl->vcol_set);
      for (Field **vf= tbl->vfield; *vf; vf++)
      {
        if (bitmap_is_set(tbl->read_set, (*vf)->field_index))
          tbl->mark_virtual_col(*vf);
      }
    }
  }

  for (cur_table= update_tables; cur_table; cur_table= cur_table->next_local)
  {
    bool can_compare_record;
    uint offset= cur_table->shared;

    table = cur_table->table;
    if (table == table_to_update)
      continue;					// Already updated
    org_updated= updated;
    tmp_table= tmp_tables[cur_table->shared];
    tmp_table->file->extra(HA_EXTRA_CACHE);	// Change to read cache
    if (unlikely((local_error= table->file->ha_rnd_init(0))))
    {
      err_table= table;
      goto err;
    }
    table->file->extra(HA_EXTRA_NO_CACHE);
    /*
      We have to clear the base record, if we have virtual indexed
      blob fields, as some storage engines will access the blob fields
      to calculate the keys to see if they have changed. Without
      clearing the blob pointers will contain random values which can
      cause a crash.
      This is a workaround for engines that access columns not present in
      either read or write set.
    */
    if (table->vfield)
      empty_record(table);

    has_vers_fields= table->vers_check_update(*fields);

    check_opt_it.rewind();
    while(TABLE *tbl= check_opt_it++)
    {
      if (unlikely((local_error= tbl->file->ha_rnd_init(0))))
      {
        err_table= tbl;
        goto err;
      }
      tbl->file->extra(HA_EXTRA_CACHE);
    }

    /*
      Setup copy functions to copy fields from temporary table
    */
    List_iterator_fast<Item> field_it(*fields_for_table[offset]);
    Field **field;
    Copy_field *copy_field_ptr= copy_field, *copy_field_end;

    /* Skip row pointers */
    field= tmp_table->field + 1 + unupdated_check_opt_tables.elements;
    for ( ; *field ; field++)
    {
      Item_field *item= (Item_field* ) field_it++;
      (copy_field_ptr++)->set(item->field, *field, 0);
    }
    copy_field_end=copy_field_ptr;

    if (unlikely((local_error= tmp_table->file->ha_rnd_init(1))))
    {
      err_table= tmp_table;
      goto err;
    }

    can_compare_record= records_are_comparable(table);

    for (;;)
    {
      if (thd->killed && trans_safe)
      {
        thd->fatal_error();
	goto err2;
      }
      if (unlikely((local_error=
                    tmp_table->file->ha_rnd_next(tmp_table->record[0]))))
      {
	if (local_error == HA_ERR_END_OF_FILE)
	  break;
        err_table= tmp_table;
	goto err;
      }

      /* call rnd_pos() using rowids from temporary table */
      check_opt_it.rewind();
      TABLE *tbl= table;
      uint field_num= 0;
      do
      {
        DBUG_ASSERT(!tmp_table->field[field_num]->is_null());
        if (unlikely((local_error=
                      tbl->file->ha_rnd_pos(tbl->record[0],
                                            (uchar *) tmp_table->
                                            field[field_num]->ptr))))
        {
          err_table= tbl;
          goto err;
        }
        field_num++;
      } while ((tbl= check_opt_it++));

      if (table->vfield &&
          unlikely(table->update_virtual_fields(table->file,
                                                VCOL_UPDATE_INDEXED_FOR_UPDATE)))
        goto err2;

      table->status|= STATUS_UPDATED;
      store_record(table,record[1]);

      /* Copy data from temporary table to current table */
      for (copy_field_ptr=copy_field;
	   copy_field_ptr != copy_field_end;
	   copy_field_ptr++)
      {
	(*copy_field_ptr->do_copy)(copy_field_ptr);
        copy_field_ptr->to_field->set_has_explicit_value();
      }

      table->evaluate_update_default_function();
      if (table->vfield &&
          table->update_virtual_fields(table->file, VCOL_UPDATE_FOR_WRITE))
        goto err2;
      if (table->triggers &&
          table->triggers->process_triggers(thd, TRG_EVENT_UPDATE,
                                            TRG_ACTION_BEFORE, TRUE))
        goto err2;

      if (!can_compare_record || compare_record(table))
      {
        int error;
        if ((error= cur_table->view_check_option(thd, ignore)) !=
            VIEW_CHECK_OK)
        {
          if (error == VIEW_CHECK_SKIP)
            continue;
          else if (unlikely(error == VIEW_CHECK_ERROR))
          {
            thd->fatal_error();
            goto err2;
          }
        }
        if (has_vers_fields && table->versioned())
          table->vers_update_fields();

        if (unlikely((local_error=
                      table->file->ha_update_row(table->record[1],
                                                 table->record[0]))) &&
            local_error != HA_ERR_RECORD_IS_THE_SAME)
	{
	  if (!ignore ||
              table->file->is_fatal_error(local_error, HA_CHECK_ALL))
          {
            err_table= table;
            goto err;
          }
        }
        if (local_error != HA_ERR_RECORD_IS_THE_SAME)
        {
          updated++;

          if (has_vers_fields && table->versioned())
          {
            if (table->versioned(VERS_TIMESTAMP))
            {
              store_record(table, record[2]);
              if ((local_error= vers_insert_history_row(table)))
              {
                restore_record(table, record[2]);
                err_table = table;
                goto err;
              }
              restore_record(table, record[2]);
            }
            updated_sys_ver++;
          }
        }
        else
          local_error= 0;
      }

      if (table->triggers &&
          unlikely(table->triggers->process_triggers(thd, TRG_EVENT_UPDATE,
                                                     TRG_ACTION_AFTER, TRUE)))
        goto err2;
    }

    if (updated != org_updated)
    {
      if (table->file->has_transactions())
        transactional_tables= TRUE;
      else
      {
        trans_safe= FALSE;				// Can't do safe rollback
        thd->transaction.stmt.modified_non_trans_table= TRUE;
      }
    }
    (void) table->file->ha_rnd_end();
    (void) tmp_table->file->ha_rnd_end();
    check_opt_it.rewind();
    while (TABLE *tbl= check_opt_it++)
        tbl->file->ha_rnd_end();

  }
  DBUG_RETURN(0);

err:
  {
    prepare_record_for_error_message(local_error, err_table);
    err_table->file->print_error(local_error,MYF(ME_FATALERROR));
  }

err2:
  if (table->file->inited)
    (void) table->file->ha_rnd_end();
  if (tmp_table->file->inited)
    (void) tmp_table->file->ha_rnd_end();
  check_opt_it.rewind();
  while (TABLE *tbl= check_opt_it++)
  {
    if (tbl->file->inited)
      (void) tbl->file->ha_rnd_end();
  }

  if (updated != org_updated)
  {
    if (table->file->has_transactions())
      transactional_tables= TRUE;
    else
    {
      trans_safe= FALSE;
      thd->transaction.stmt.modified_non_trans_table= TRUE;
    }
  }
  DBUG_RETURN(1);
}