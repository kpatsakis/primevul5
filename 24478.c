int ha_maria::assign_to_keycache(THD * thd, HA_CHECK_OPT *check_opt)
{
#if 0 && NOT_IMPLEMENTED
  PAGECACHE *new_pagecache= check_opt->pagecache;
  const char *errmsg= 0;
  int error= HA_ADMIN_OK;
  ulonglong map;
  TABLE_LIST *table_list= table->pos_in_table_list;
  DBUG_ENTER("ha_maria::assign_to_keycache");

  table->keys_in_use_for_query.clear_all();

  if (table_list->process_index_hints(table))
    DBUG_RETURN(HA_ADMIN_FAILED);
  map= ~(ulonglong) 0;
  if (!table->keys_in_use_for_query.is_clear_all())
    /* use all keys if there's no list specified by the user through hints */
    map= table->keys_in_use_for_query.to_ulonglong();

  if ((error= maria_assign_to_pagecache(file, map, new_pagecache)))
  {
    char buf[STRING_BUFFER_USUAL_SIZE];
    my_snprintf(buf, sizeof(buf),
                "Failed to flush to index file (errno: %d)", error);
    errmsg= buf;
    error= HA_ADMIN_CORRUPT;
  }

  if (error != HA_ADMIN_OK)
  {
    /* Send error to user */
    HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);
    if (!param)
      return HA_ADMIN_INTERNAL_ERROR;

    maria_chk_init(param);
    param->thd= thd;
    param->op_name= "assign_to_keycache";
    param->db_name= table->s->db.str;
    param->table_name= table->s->table_name.str;
    param->testflag= 0;
    _ma_check_print_error(param, errmsg);
  }
  DBUG_RETURN(error);
#else
  return  HA_ADMIN_NOT_IMPLEMENTED;
#endif
}