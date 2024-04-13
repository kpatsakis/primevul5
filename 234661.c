void JOIN_TAB::partial_cleanup()
{
  if (!table)
    return;

  if (table->is_created())
  {
    table->file->ha_index_or_rnd_end();
    DBUG_PRINT("info", ("close index: %s.%s  alias: %s",
               table->s->db.str,
               table->s->table_name.str,
               table->alias.c_ptr()));
    if (aggr)
    {
      int tmp= 0;
      if ((tmp= table->file->extra(HA_EXTRA_NO_CACHE)))
        table->file->print_error(tmp, MYF(0));
    }
  }
  delete filesort_result;
  filesort_result= NULL;
  free_cache(&read_record);
}