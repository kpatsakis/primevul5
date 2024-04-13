ulong check_selectivity(THD *thd,
                        ulong rows_to_read,
                        TABLE *table,
                        List<COND_STATISTIC> *conds)
{
  ulong count= 0;
  COND_STATISTIC *cond;
  List_iterator_fast<COND_STATISTIC> it(*conds);
  handler *file= table->file;
  uchar *record= table->record[0];
  int error= 0;
  DBUG_ENTER("check_selectivity");

  DBUG_ASSERT(rows_to_read > 0);
  while ((cond= it++))
  {
    DBUG_ASSERT(cond->cond);
    DBUG_ASSERT(cond->cond->used_tables() == table->map);
    cond->positive= 0;
  }
  it.rewind();

  if (file->ha_rnd_init_with_error(1))
    DBUG_RETURN(0);
  do
  {
    error= file->ha_rnd_next(record);

    if (thd->killed)
    {
      thd->send_kill_message();
      count= 0;
      goto err;
    }
    if (error)
    {
      if (error == HA_ERR_RECORD_DELETED)
        continue;
      if (error == HA_ERR_END_OF_FILE)
	break;
      goto err;
    }

    count++;
    while ((cond= it++))
    {
      if (cond->cond->val_bool())
        cond->positive++;
    }
    it.rewind();

  } while (count < rows_to_read);

  file->ha_rnd_end();
  DBUG_RETURN(count);

err:
  DBUG_PRINT("error", ("error %d", error));
  file->ha_rnd_end();
  DBUG_RETURN(0);
}