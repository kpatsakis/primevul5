void ha_maria::start_bulk_insert(ha_rows rows, uint flags)
{
  DBUG_ENTER("ha_maria::start_bulk_insert");
  THD *thd= table->in_use;
  MARIA_SHARE *share= file->s;
  bool index_disabled= 0;
  DBUG_PRINT("info", ("start_bulk_insert: rows %lu", (ulong) rows));

  /* don't enable row cache if too few rows */
  if ((!rows || rows > MARIA_MIN_ROWS_TO_USE_WRITE_CACHE) && !has_long_unique())
  {
    ulonglong size= thd->variables.read_buff_size, tmp;
    if (rows)
    {
      if (file->state->records)
      {
        MARIA_INFO maria_info;
        maria_status(file, &maria_info, HA_STATUS_NO_LOCK |HA_STATUS_VARIABLE);
        set_if_smaller(size, maria_info.mean_reclength * rows);
      }
      else if (table->s->avg_row_length)
        set_if_smaller(size, (size_t) (table->s->avg_row_length * rows));
    }
    tmp= (ulong) size;                          // Safe becasue of limits
    maria_extra(file, HA_EXTRA_WRITE_CACHE, (void*) &tmp);
  }

  can_enable_indexes= (maria_is_all_keys_active(share->state.key_map,
                                                share->base.keys));
  bulk_insert_single_undo= BULK_INSERT_NONE;

  if (!(specialflag & SPECIAL_SAFE_MODE))
  {
    /*
       Only disable old index if the table was empty and we are inserting
       a lot of rows.
       We should not do this for only a few rows as this is slower and
       we don't want to update the key statistics based of only a few rows.
       Index file rebuild requires an exclusive lock, so if versioning is on
       don't do it (see how ha_maria::store_lock() tries to predict repair).
       We can repair index only if we have an exclusive (TL_WRITE) lock or
       if this is inside an ALTER TABLE, in which case lock_type == TL_UNLOCK.

       To see if table is empty, we shouldn't rely on the old record
       count from our transaction's start (if that old count is 0 but
       now there are records in the table, we would wrongly destroy
       them).  So we need to look at share->state.state.records.  As a
       safety net for now, we don't remove the test of
       file->state->records, because there is uncertainty on what will
       happen during repair if the two states disagree.

       We also have to check in case of transactional tables that the
       user has not used LOCK TABLE on the table twice.
    */
    if ((file->state->records == 0) &&
        (share->state.state.records == 0) && can_enable_indexes &&
        (!rows || rows >= MARIA_MIN_ROWS_TO_DISABLE_INDEXES) &&
        (file->lock.type == TL_WRITE || file->lock.type == TL_UNLOCK) &&
        (!share->have_versioning || !share->now_transactional ||
         file->used_tables->use_count == 1))
    {
      /**
         @todo for a single-row INSERT SELECT, we will go into repair, which
         is more costly (flushes, syncs) than a row write.
      */
      if (file->open_flags & HA_OPEN_INTERNAL_TABLE)
      {
        /* Internal table; If we get a duplicate something is very wrong */
        file->update|= HA_STATE_CHANGED;
        index_disabled= share->base.keys > 0;
        maria_clear_all_keys_active(file->s->state.key_map);
      }
      else
      {
        my_bool all_keys= MY_TEST(flags & HA_CREATE_UNIQUE_INDEX_BY_SORT);
        /*
          Deactivate all indexes that can be recreated fast.
          These include packed keys on which sorting will use more temporary
          space than the max allowed file length or for which the unpacked keys
          will take much more space than packed keys.
          Note that 'rows' may be zero for the case when we don't know how many
          rows we will put into the file.
        */
        MARIA_SHARE *share= file->s;
        MARIA_KEYDEF    *key=share->keyinfo;
        uint          i;

        DBUG_ASSERT(share->state.state.records == 0 &&
                    (!rows || rows >= MARIA_MIN_ROWS_TO_DISABLE_INDEXES));
        for (i=0 ; i < share->base.keys ; i++,key++)
        {
          if (!(key->flag & (HA_SPATIAL | HA_AUTO_KEY | HA_RTREE_INDEX)) &&
              ! maria_too_big_key_for_sort(key,rows) && share->base.auto_key != i+1 &&
              (all_keys || !(key->flag & HA_NOSAME)) &&
              table->key_info[i].algorithm != HA_KEY_ALG_LONG_HASH)
          {
            maria_clear_key_active(share->state.key_map, i);
            index_disabled= 1;
            file->update|= HA_STATE_CHANGED;
            file->create_unique_index_by_sort= all_keys;
          }
        }
      }
      if (share->now_transactional)
      {
        bulk_insert_single_undo= BULK_INSERT_SINGLE_UNDO_AND_NO_REPAIR;
        write_log_record_for_bulk_insert(file);
        _ma_tmp_disable_logging_for_table(file, TRUE);
        /*
          Pages currently in the page cache have type PAGECACHE_LSN_PAGE, we
          are not allowed to overwrite them with PAGECACHE_PLAIN_PAGE, so
          throw them away. It is not losing data, because we just wrote and
          forced an UNDO which will for sure empty the table if we crash. The
          upcoming unique-key insertions however need a proper index, so we
          cannot leave the corrupted on-disk index file, thus we truncate it.
        */
        maria_delete_all_rows(file);
      }
    }
    else if (!file->bulk_insert &&
             (!rows || rows >= MARIA_MIN_ROWS_TO_USE_BULK_INSERT))
    {
      maria_init_bulk_insert(file,
                             (size_t) thd->variables.bulk_insert_buff_size,
                             rows);
    }
  }
  can_enable_indexes= index_disabled;
  DBUG_VOID_RETURN;
}