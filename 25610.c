bool instantiate_tmp_table(TABLE *table, KEY *keyinfo, 
                           TMP_ENGINE_COLUMNDEF *start_recinfo,
                           TMP_ENGINE_COLUMNDEF **recinfo,
                           ulonglong options)
{
  if (table->s->db_type() == TMP_ENGINE_HTON)
  {
    if (create_internal_tmp_table(table, keyinfo, start_recinfo, recinfo,
                                  options))
      return TRUE;
    // Make empty record so random data is not written to disk
    empty_record(table);
    table->status= STATUS_NO_RECORD;
  }
  if (open_tmp_table(table))
    return TRUE;

  return FALSE;
}