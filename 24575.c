int ha_maria::open(const char *name, int mode, uint test_if_locked)
{
  uint i;

#ifdef NOT_USED
  /*
    If the user wants to have memory mapped data files, add an
    open_flag. Do not memory map temporary tables because they are
    expected to be inserted and thus extended a lot. Memory mapping is
    efficient for files that keep their size, but very inefficient for
    growing files. Using an open_flag instead of calling ma_extra(...
    HA_EXTRA_MMAP ...) after maxs_open() has the advantage that the
    mapping is not repeated for every open, but just done on the initial
    open, when the MyISAM share is created. Every time the server
    requires to open a new instance of a table it calls this method. We
    will always supply HA_OPEN_MMAP for a permanent table. However, the
    Maria storage engine will ignore this flag if this is a secondary
    open of a table that is in use by other threads already (if the
    Maria share exists already).
  */
  if (!(test_if_locked & HA_OPEN_TMP_TABLE) && opt_maria_use_mmap)
    test_if_locked|= HA_OPEN_MMAP;
#endif

  if (maria_recover_options & HA_RECOVER_ANY)
  {
    /* user asked to trigger a repair if table was not properly closed */
    test_if_locked|= HA_OPEN_ABORT_IF_CRASHED;
  }

  if (aria_readonly)
    test_if_locked|= HA_OPEN_IGNORE_MOVED_STATE;

  if (!(file= maria_open(name, mode, test_if_locked | HA_OPEN_FROM_SQL_LAYER,
                         s3_open_args())))
  {
    if (my_errno == HA_ERR_OLD_FILE)
    {
      push_warning(current_thd, Sql_condition::WARN_LEVEL_NOTE,
                   ER_CRASHED_ON_USAGE,
                   zerofill_error_msg);
    }
    return (my_errno ? my_errno : -1);
  }
  if (aria_readonly)
    file->s->options|= HA_OPTION_READ_ONLY_DATA;

  file->s->chst_invalidator= query_cache_invalidate_by_MyISAM_filename_ref;
  /* Set external_ref, mainly for temporary tables */
  file->external_ref= (void*) table;            // For ma_killed()

  if (test_if_locked & (HA_OPEN_IGNORE_IF_LOCKED | HA_OPEN_TMP_TABLE))
    maria_extra(file, HA_EXTRA_NO_WAIT_LOCK, 0);

  info(HA_STATUS_NO_LOCK | HA_STATUS_VARIABLE | HA_STATUS_CONST);
  if (!(test_if_locked & HA_OPEN_WAIT_IF_LOCKED))
    maria_extra(file, HA_EXTRA_WAIT_LOCK, 0);
  if ((data_file_type= file->s->data_file_type) != STATIC_RECORD)
    int_table_flags |= HA_REC_NOT_IN_SEQ;
  if (!file->s->base.born_transactional)
  {
    /*
      INSERT DELAYED cannot work with transactional tables (because it cannot
      stand up to "when client gets ok the data is safe on disk": the record
      may not even be inserted). In the future, we could enable it back (as a
      client doing INSERT DELAYED knows the specificities; but we then should
      make sure to regularly commit in the delayed_insert thread).
    */
    int_table_flags|= HA_CAN_INSERT_DELAYED | HA_NO_TRANSACTIONS;
  }
  else
    int_table_flags|= HA_CRASH_SAFE;

  if (file->s->options & (HA_OPTION_CHECKSUM | HA_OPTION_COMPRESS_RECORD))
    int_table_flags |= HA_HAS_NEW_CHECKSUM;

  /*
    We can only do online backup on transactional tables with checksum.
    Checksums are needed to avoid half writes.
  */
  if (file->s->options & HA_OPTION_PAGE_CHECKSUM &&
      file->s->base.born_transactional)
    int_table_flags |= HA_CAN_ONLINE_BACKUPS;

  /*
    For static size rows, tell MariaDB that we will access all bytes
    in the record when writing it.  This signals MariaDB to initialize
    the full row to ensure we don't get any errors from valgrind and
    that all bytes in the row is properly reset.
  */
  if (file->s->data_file_type == STATIC_RECORD &&
      (file->s->has_varchar_fields || file->s->has_null_fields))
    int_table_flags|= HA_RECORD_MUST_BE_CLEAN_ON_WRITE;

  for (i= 0; i < table->s->keys; i++)
  {
    plugin_ref parser= table->key_info[i].parser;
    if (table->key_info[i].flags & HA_USES_PARSER)
      file->s->keyinfo[i].parser=
        (struct st_mysql_ftparser *)plugin_decl(parser)->info;
    table->key_info[i].block_size= file->s->keyinfo[i].block_length;
  }
  my_errno= 0;

  /* Count statistics of usage for newly open normal files */
  if (file->s->reopen == 1 && ! (test_if_locked & HA_OPEN_TMP_TABLE))
  {
    if (file->s->delay_key_write)
      feature_files_opened_with_delayed_keys++;
  }

  return my_errno;
}