optimize_straight_join(JOIN *join, table_map join_tables)
{
  JOIN_TAB *s;
  uint idx= join->const_tables;
  bool disable_jbuf= join->thd->variables.join_cache_level == 0;
  double    record_count= 1.0;
  double    read_time=    0.0;
  uint use_cond_selectivity= 
         join->thd->variables.optimizer_use_condition_selectivity;
  POSITION  loose_scan_pos;

  for (JOIN_TAB **pos= join->best_ref + idx ; (s= *pos) ; pos++)
  {
    /* Find the best access method from 's' to the current partial plan */
    best_access_path(join, s, join_tables, join->positions, idx,
                     disable_jbuf, record_count,
                     join->positions + idx, &loose_scan_pos);

    /* compute the cost of the new plan extended with 's' */
    record_count= COST_MULT(record_count, join->positions[idx].records_read);
    read_time= COST_ADD(read_time,
                        COST_ADD(join->positions[idx].read_time,
                                 record_count / (double) TIME_FOR_COMPARE));
    advance_sj_state(join, join_tables, idx, &record_count, &read_time,
                     &loose_scan_pos);

    join_tables&= ~(s->table->map);
    double pushdown_cond_selectivity= 1.0;
    if (use_cond_selectivity > 1)
      pushdown_cond_selectivity= table_cond_selectivity(join, idx, s,
                                                        join_tables);
    join->positions[idx].cond_selectivity= pushdown_cond_selectivity;
    ++idx;
  }

  if (join->sort_by_table &&
      join->sort_by_table != join->positions[join->const_tables].table->table)
    read_time+= record_count;  // We have to make a temp table
  memcpy((uchar*) join->best_positions, (uchar*) join->positions,
         sizeof(POSITION)*idx);
  join->join_record_count= record_count;
  join->best_read= read_time - 0.001;
}