int ha_maria::find_unique_row(uchar *record, uint constrain_no)
{
  int rc;
  register_handler(file);
  if (file->s->state.header.uniques)
  {
    DBUG_ASSERT(file->s->state.header.uniques > constrain_no);
    MARIA_UNIQUEDEF *def= file->s->uniqueinfo + constrain_no;
    ha_checksum unique_hash= _ma_unique_hash(def, record);
    rc= _ma_check_unique(file, def, record, unique_hash, HA_OFFSET_ERROR);
    if (rc)
    {
      file->cur_row.lastpos= file->dup_key_pos;
      if ((*file->read_record)(file, record, file->cur_row.lastpos))
        return -1;
      file->update|= HA_STATE_AKTIV;                     /* Record is read */
    }
    // invert logic
    rc= !MY_TEST(rc);
  }
  else
  {
    /*
     It is case when just unique index used instead unicue constrain
     (conversion from heap table).
     */
    DBUG_ASSERT(file->s->state.header.keys > constrain_no);
    MARIA_KEY key;
    file->once_flags|= USE_PACKED_KEYS;
    (*file->s->keyinfo[constrain_no].make_key)
      (file, &key, constrain_no, file->lastkey_buff2, record, 0, 0);
    rc= maria_rkey(file, record, constrain_no, key.data, key.data_length,
                   HA_READ_KEY_EXACT);
    rc= MY_TEST(rc);
  }
  return rc;
}