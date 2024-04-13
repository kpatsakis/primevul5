int ha_maria::info(uint flag)
{
  MARIA_INFO maria_info;
  char name_buff[FN_REFLEN];

  (void) maria_status(file, &maria_info, flag);
  if (flag & HA_STATUS_VARIABLE)
  {
    stats.records=           maria_info.records;
    stats.deleted=           maria_info.deleted;
    stats.data_file_length=  maria_info.data_file_length;
    stats.index_file_length= maria_info.index_file_length;
    stats.delete_length=     maria_info.delete_length;
    stats.check_time=        maria_info.check_time;
    stats.mean_rec_length=   maria_info.mean_reclength;
    stats.checksum=          file->state->checksum;
  }
  if (flag & HA_STATUS_CONST)
  {
    TABLE_SHARE *share= table->s;
    stats.max_data_file_length=  maria_info.max_data_file_length;
    stats.max_index_file_length= maria_info.max_index_file_length;
    stats.create_time= maria_info.create_time;
    ref_length= maria_info.reflength;
    share->db_options_in_use= maria_info.options;
    stats.block_size= maria_block_size;
    stats.mrr_length_per_rec= maria_info.reflength + 8; // 8 = MY_MAX(sizeof(void *))

    /* Update share */
    share->keys_in_use.set_prefix(share->keys);
    share->keys_in_use.intersect_extended(maria_info.key_map);
    share->keys_for_keyread.intersect(share->keys_in_use);
    share->db_record_offset= maria_info.record_offset;
    if (share->key_parts)
    {
      ulong *to= table->key_info[0].rec_per_key, *end;
      double *from= maria_info.rec_per_key;
      for (end= to+ share->key_parts ; to < end ; to++, from++)
        *to= (ulong) (*from + 0.5);
    }

    /*
       Set data_file_name and index_file_name to point at the symlink value
       if table is symlinked (Ie;  Real name is not same as generated name)
    */
    data_file_name= index_file_name= 0;
    fn_format(name_buff, file->s->open_file_name.str, "", MARIA_NAME_DEXT,
              MY_APPEND_EXT | MY_UNPACK_FILENAME);
    if (strcmp(name_buff, maria_info.data_file_name) &&
        maria_info.data_file_name[0])
      data_file_name= maria_info.data_file_name;
    fn_format(name_buff, file->s->open_file_name.str, "", MARIA_NAME_IEXT,
              MY_APPEND_EXT | MY_UNPACK_FILENAME);
    if (strcmp(name_buff, maria_info.index_file_name) &&
        maria_info.index_file_name[0])
      index_file_name=maria_info.index_file_name;
  }
  if (flag & HA_STATUS_ERRKEY)
  {
    errkey= maria_info.errkey;
    my_store_ptr(dup_ref, ref_length, maria_info.dup_key_pos);
  }
  if (flag & HA_STATUS_TIME)
    stats.update_time= maria_info.update_time;
  if (flag & HA_STATUS_AUTO)
    stats.auto_increment_value= maria_info.auto_increment;

  return 0;
}