int ha_maria::create(const char *name, TABLE *table_arg,
                     HA_CREATE_INFO *ha_create_info)
{
  int error;
  uint create_flags= 0, record_count= 0, i;
  char buff[FN_REFLEN];
  MARIA_KEYDEF *keydef;
  MARIA_COLUMNDEF *recinfo;
  MARIA_CREATE_INFO create_info;
  TABLE_SHARE *share= table_arg->s;
  uint options= share->db_options_in_use;
  ha_table_option_struct *table_options= table_arg->s->option_struct;
  enum data_file_type row_type;
  THD *thd= current_thd;
  DBUG_ENTER("ha_maria::create");

  for (i= 0; i < share->keys; i++)
  {
    if (table_arg->key_info[i].flags & HA_USES_PARSER)
    {
      create_flags|= HA_CREATE_RELIES_ON_SQL_LAYER;
      break;
    }
  }
  /* Note: BLOCK_RECORD is used if table is transactional */
  row_type= maria_row_type(ha_create_info);
  if (ha_create_info->transactional == HA_CHOICE_YES &&
      ha_create_info->row_type != ROW_TYPE_PAGE &&
      ha_create_info->row_type != ROW_TYPE_NOT_USED &&
      ha_create_info->row_type != ROW_TYPE_DEFAULT)
    push_warning(thd, Sql_condition::WARN_LEVEL_NOTE,
                 ER_ILLEGAL_HA_CREATE_OPTION,
                 "Row format set to PAGE because of TRANSACTIONAL=1 option");

  if (share->table_type == TABLE_TYPE_SEQUENCE)
  {
    /* For sequences, the simples record type is appropriate */
    row_type= STATIC_RECORD;
    ha_create_info->transactional= HA_CHOICE_NO;
  }

  bzero((char*) &create_info, sizeof(create_info));
  if ((error= table2maria(table_arg, row_type, &keydef, &recinfo,
                          &record_count, &create_info)))
    DBUG_RETURN(error); /* purecov: inspected */
  create_info.max_rows= share->max_rows;
  create_info.reloc_rows= share->min_rows;
  create_info.with_auto_increment= share->next_number_key_offset == 0;
  create_info.auto_increment= (ha_create_info->auto_increment_value ?
                               ha_create_info->auto_increment_value -1 :
                               (ulonglong) 0);
  create_info.data_file_length= ((ulonglong) share->max_rows *
                                 share->avg_row_length);
  create_info.data_file_name= ha_create_info->data_file_name;
  create_info.index_file_name= ha_create_info->index_file_name;
  create_info.language= share->table_charset->number;
  if (ht != maria_hton)
  {
    /* S3 engine */
    create_info.s3_block_size= (ulong) table_options->s3_block_size;
    create_info.compression_algorithm= table_options->compression_algorithm;
  }

  /*
    Table is transactional:
    - If the user specify that table is transactional (in this case
      row type is forced to BLOCK_RECORD)
    - If they specify BLOCK_RECORD without specifying transactional behaviour

    Shouldn't this test be pushed down to maria_create()? Because currently,
    ma_test1 -T crashes: it creates a table with DYNAMIC_RECORD but has
    born_transactional==1, which confuses some recovery-related code.
  */
  create_info.transactional= (row_type == BLOCK_RECORD &&
                              ha_create_info->transactional != HA_CHOICE_NO);

  if (ha_create_info->tmp_table())
  {
    create_flags|= HA_CREATE_TMP_TABLE | HA_CREATE_DELAY_KEY_WRITE;
    create_info.transactional= 0;
  }
  if (ha_create_info->options & HA_CREATE_KEEP_FILES)
    create_flags|= HA_CREATE_KEEP_FILES;
  if (options & HA_OPTION_PACK_RECORD)
    create_flags|= HA_PACK_RECORD;
  if (options & HA_OPTION_CHECKSUM)
    create_flags|= HA_CREATE_CHECKSUM;
  if (options & HA_OPTION_DELAY_KEY_WRITE)
    create_flags|= HA_CREATE_DELAY_KEY_WRITE;
  if ((ha_create_info->page_checksum == HA_CHOICE_UNDEF &&
       maria_page_checksums) ||
       ha_create_info->page_checksum ==  HA_CHOICE_YES)
    create_flags|= HA_CREATE_PAGE_CHECKSUM;

  (void) translog_log_debug_info(0, LOGREC_DEBUG_INFO_QUERY,
                                 (uchar*) thd->query(), thd->query_length());

  create_info.encrypted= maria_encrypt_tables && ht == maria_hton;
  /* TODO: Check that the following fn_format is really needed */
  error=
    maria_create(fn_format(buff, name, "", "",
                           MY_UNPACK_FILENAME | MY_APPEND_EXT),
                 row_type, share->keys, keydef,
                 record_count,  recinfo,
                 0, (MARIA_UNIQUEDEF *) 0,
                 &create_info, create_flags);

  my_free(recinfo);
  DBUG_RETURN(error);
}