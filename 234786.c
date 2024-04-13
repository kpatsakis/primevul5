double JOIN_TAB::scan_time()
{
  double res;
  if (table->is_created())
  {
    if (table->is_filled_at_execution())
    {
      get_delayed_table_estimates(table, &records, &read_time,
                                    &startup_cost);
      found_records= records;
      table->quick_condition_rows= records;
    }
    else
    {
      found_records= records= table->stat_records();
      read_time= table->file->scan_time();
      /*
        table->quick_condition_rows has already been set to
        table->file->stats.records
      */
    }
    res= read_time;
  }
  else
  {
    found_records= records=table->stat_records();
    read_time= found_records ? (double)found_records: 10.0;// TODO:fix this stub
    res= read_time;
  }
  return res;
}