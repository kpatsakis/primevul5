static bool open_table_entry_fini(THD *thd, TABLE_SHARE *share, TABLE *entry)
{
  if (Table_triggers_list::check_n_load(thd, share->db.str,
                                        share->table_name.str, entry, 0))
    return TRUE;

  /*
    If we are here, there was no fatal error (but error may be still
    unitialized).
  */
  if (unlikely(entry->file->implicit_emptied))
  {
    entry->file->implicit_emptied= 0;
    if (mysql_bin_log.is_open())
    {
      char query_buf[2*FN_REFLEN + 21];
      String query(query_buf, sizeof(query_buf), system_charset_info);

      query.length(0);
      query.append("DELETE FROM ");
      append_identifier(thd, &query, share->db.str, share->db.length);
      query.append(".");
      append_identifier(thd, &query, share->table_name.str,
                          share->table_name.length);

      /*
        we bypass thd->binlog_query() here,
        as it does a lot of extra work, that is simply wrong in this case
      */
      Query_log_event qinfo(thd, query.ptr(), query.length(),
                            FALSE, TRUE, TRUE, 0);
      if (mysql_bin_log.write(&qinfo))
        return TRUE;
    }
  }
  return FALSE;
}