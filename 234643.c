static int remove_dup_with_compare(THD *thd, TABLE *table, Field **first_field,
				   Item *having)
{
  handler *file=table->file;
  uchar *record=table->record[0];
  int error;
  DBUG_ENTER("remove_dup_with_compare");

  if (unlikely(file->ha_rnd_init_with_error(1)))
    DBUG_RETURN(1);

  error= file->ha_rnd_next(record);
  for (;;)
  {
    if (unlikely(thd->check_killed()))
    {
      error=0;
      goto err;
    }
    if (unlikely(error))
    {
      if (error == HA_ERR_END_OF_FILE)
	break;
      goto err;
    }
    if (having && !having->val_int())
    {
      if (unlikely((error= file->ha_delete_row(record))))
	goto err;
      error= file->ha_rnd_next(record);
      continue;
    }
    if (unlikely(copy_blobs(first_field)))
    {
      my_message(ER_OUTOFMEMORY, ER_THD(thd,ER_OUTOFMEMORY),
                 MYF(ME_FATALERROR));
      error=0;
      goto err;
    }
    store_record(table,record[1]);

    /* Read through rest of file and mark duplicated rows deleted */
    bool found=0;
    for (;;)
    {
      if (unlikely((error= file->ha_rnd_next(record))))
      {
	if (error == HA_ERR_END_OF_FILE)
	  break;
	goto err;
      }
      if (compare_record(table, first_field) == 0)
      {
	if (unlikely((error= file->ha_delete_row(record))))
	  goto err;
      }
      else if (!found)
      {
	found=1;
        if (unlikely((error= file->remember_rnd_pos())))
          goto err;
      }
    }
    if (!found)
      break;					// End of file
    /* Restart search on saved row */
    if (unlikely((error= file->restart_rnd_next(record))))
      goto err;
  }

  file->extra(HA_EXTRA_NO_CACHE);
  (void) file->ha_rnd_end();
  DBUG_RETURN(0);
err:
  file->extra(HA_EXTRA_NO_CACHE);
  (void) file->ha_rnd_end();
  if (error)
    file->print_error(error,MYF(0));
  DBUG_RETURN(1);
}