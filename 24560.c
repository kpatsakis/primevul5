int ha_maria::zerofill(THD * thd, HA_CHECK_OPT *check_opt)
{
  int error;
  HA_CHECK *param= (HA_CHECK*) thd->alloc(sizeof *param);
  TRN *old_trn;
  MARIA_SHARE *share= file->s;

  if (!file || !param)
    return HA_ADMIN_INTERNAL_ERROR;

  unmap_file(file);
  old_trn= file->trn;
  maria_chk_init(param);
  param->thd= thd;
  param->op_name= "zerofill";
  param->testflag= check_opt->flags | T_SILENT | T_ZEROFILL;
  param->orig_sort_buffer_length= THDVAR(thd, sort_buffer_size);
  param->db_name= table->s->db.str;
  param->table_name= table->alias.c_ptr();

  error=maria_zerofill(param, file, share->open_file_name.str);

  /* Reset trn, that may have been set by repair */
  if (old_trn && old_trn != file->trn)
    _ma_set_trn_for_table(file, old_trn);

  if (!error)
  {
    TrID create_trid= trnman_get_min_safe_trid();
    mysql_mutex_lock(&share->intern_lock);
    share->state.changed|= STATE_NOT_MOVABLE;
    maria_update_state_info(param, file, UPDATE_TIME | UPDATE_OPEN_COUNT);
    _ma_update_state_lsns_sub(share, LSN_IMPOSSIBLE, create_trid,
                              TRUE, TRUE);
    mysql_mutex_unlock(&share->intern_lock);
  }
  return error;
}