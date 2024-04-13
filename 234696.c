double matching_candidates_in_table(JOIN_TAB *s, bool with_found_constraint,
                                     uint use_cond_selectivity)
{
  ha_rows records;
  double dbl_records;

  if (use_cond_selectivity > 1)
  {
    TABLE *table= s->table;
    double sel= table->cond_selectivity;
    double table_records= (double)table->stat_records();
    dbl_records= table_records * sel;
    return dbl_records;
  }

  records = s->found_records;

  /*
    If there is a filtering condition on the table (i.e. ref analyzer found
    at least one "table.keyXpartY= exprZ", where exprZ refers only to tables
    preceding this table in the join order we're now considering), then 
    assume that 25% of the rows will be filtered out by this condition.

    This heuristic is supposed to force tables used in exprZ to be before
    this table in join order.
  */
  if (with_found_constraint)
    records-= records/4;

    /*
      If applicable, get a more accurate estimate. Don't use the two
      heuristics at once.
    */
  if (s->table->quick_condition_rows != s->found_records)
    records= s->table->quick_condition_rows;

  dbl_records= (double)records;
  return dbl_records;
}