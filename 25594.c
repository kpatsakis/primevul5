void JOIN::get_prefix_cost_and_fanout(uint n_tables, 
                                      double *read_time_arg,
                                      double *record_count_arg)
{
  double record_count= 1;
  double read_time= 0.0;
  for (uint i= const_tables; i < n_tables + const_tables ; i++)
  {
    if (best_positions[i].records_read)
    {
      record_count= COST_MULT(record_count, best_positions[i].records_read);
      read_time= COST_ADD(read_time, best_positions[i].read_time);
    }
  }
  *read_time_arg= read_time;// + record_count / TIME_FOR_COMPARE;
  *record_count_arg= record_count;
}