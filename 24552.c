int ha_maria::preload_keys(THD * thd, HA_CHECK_OPT *check_opt)
{
  ulonglong map;
  TABLE_LIST *table_list= table->pos_in_table_list;

  DBUG_ENTER("ha_maria::preload_keys");

  table->keys_in_use_for_query.clear_all();

  if (table_list->process_index_hints(table))
    DBUG_RETURN(HA_ADMIN_FAILED);

  map= ~(ulonglong) 0;
  /* Check validity of the index references */
  if (!table->keys_in_use_for_query.is_clear_all())
    /* use all keys if there's no list specified by the user through hints */
    map= table->keys_in_use_for_query.to_ulonglong();

  maria_extra(file, HA_EXTRA_PRELOAD_BUFFER_SIZE,
              (void*) &thd->variables.preload_buff_size);

  int error;

  if ((error= maria_preload(file, map, table_list->ignore_leaves)))
  {
    char buf[MYSQL_ERRMSG_SIZE+20];
    const char *errmsg;

    switch (error) {
    case HA_ERR_NON_UNIQUE_BLOCK_SIZE:
      errmsg= "Indexes use different block sizes";
      break;
    case HA_ERR_OUT_OF_MEM:
      errmsg= "Failed to allocate buffer";
      break;
    default:
      my_snprintf(buf, sizeof(buf),
                  "Failed to read from index file (errno: %d)", my_errno);
      errmsg= buf;
    }

    HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);
    if (!param)
      return HA_ADMIN_INTERNAL_ERROR;

    maria_chk_init(param);
    param->thd= thd;
    param->op_name= "preload_keys";
    param->db_name= table->s->db.str;
    param->table_name= table->s->table_name.str;
    param->testflag= 0;
    _ma_check_print_error(param, "%s", errmsg);
    DBUG_RETURN(HA_ADMIN_FAILED);
  }
  DBUG_RETURN(HA_ADMIN_OK);
}