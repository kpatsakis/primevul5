static int ha_maria_init(void *p)
{
  int res= 0, tmp;
  const char *log_dir= maria_data_root;

  /*
    If aria_readonly is set, then we don't run recovery and we don't allow
    opening of tables that are crashed. Used by mysqld --help
   */
  if ((aria_readonly= opt_help != 0))
  {
    maria_recover_options= 0;
    checkpoint_interval= 0;
  }

#ifdef HAVE_PSI_INTERFACE
  init_aria_psi_keys();
#endif

  maria_hton= (handlerton *)p;
  maria_hton->db_type= DB_TYPE_ARIA;
  maria_hton->create= maria_create_handler;
  maria_hton->panic= maria_hton_panic;
  maria_hton->tablefile_extensions= ha_maria_exts;
  maria_hton->commit= maria_commit;
  maria_hton->rollback= maria_rollback;
  maria_hton->checkpoint_state= maria_checkpoint_state;
  maria_hton->flush_logs= maria_flush_logs;
  maria_hton->show_status= maria_show_status;
  maria_hton->prepare_for_backup= maria_prepare_for_backup;
  maria_hton->end_backup= maria_end_backup;

  /* TODO: decide if we support Maria being used for log tables */
  maria_hton->flags= (HTON_CAN_RECREATE | HTON_SUPPORT_LOG_TABLES |
                      HTON_NO_ROLLBACK |
                      HTON_TRANSACTIONAL_AND_NON_TRANSACTIONAL);
  bzero(maria_log_pagecache, sizeof(*maria_log_pagecache));
  maria_tmpdir= &mysql_tmpdir_list;             /* For REDO */

  if (!aria_readonly)
    res= maria_upgrade();
  res= res || maria_init();
  tmp= ma_control_file_open(!aria_readonly, !aria_readonly, !aria_readonly);
  res= res || aria_readonly ? tmp == CONTROL_FILE_LOCKED : tmp != 0;
  res= res ||
    ((force_start_after_recovery_failures != 0 && !aria_readonly) &&
     mark_recovery_start(log_dir)) ||
    !init_pagecache(maria_pagecache,
                    (size_t) pagecache_buffer_size, pagecache_division_limit,
                    pagecache_age_threshold, maria_block_size, pagecache_file_hash_size,
                    0) ||
    !init_pagecache(maria_log_pagecache,
                    TRANSLOG_PAGECACHE_SIZE, 0, 0,
                    TRANSLOG_PAGE_SIZE, 0, 0) ||
    (!aria_readonly &&
     translog_init(maria_data_root, log_file_size,
                   MYSQL_VERSION_ID, server_id, maria_log_pagecache,
                   TRANSLOG_DEFAULT_FLAGS, 0)) ||
    (!aria_readonly &&
     (maria_recovery_from_log() ||
      ((force_start_after_recovery_failures != 0 ||
        maria_recovery_changed_data || recovery_failures) &&
       mark_recovery_success()))) ||
    (aria_readonly && trnman_init(MAX_INTERNAL_TRID-16)) ||
    ma_checkpoint_init(checkpoint_interval);
  maria_multi_threaded= maria_in_ha_maria= TRUE;
  maria_create_trn_hook= maria_create_trn_for_mysql;
  maria_pagecache->extra_debug= 1;
  maria_assert_if_crashed_table= debug_assert_if_crashed_table;

  if (res)
  {
    maria_hton= 0;
    maria_panic(HA_PANIC_CLOSE);
  }

  ma_killed= ma_killed_in_mariadb;
  if (res)
    maria_panic(HA_PANIC_CLOSE);

  return res ? HA_ERR_INITIALIZATION : 0;
}