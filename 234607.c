double JOIN::get_examined_rows()
{
  double examined_rows;
  double prev_fanout= 1;
  double records;
  JOIN_TAB *tab= first_breadth_first_tab();
  JOIN_TAB *prev_tab= tab;

  records= (double)tab->get_examined_rows();

  while ((tab= next_breadth_first_tab(first_breadth_first_tab(),
                                      top_join_tab_count, tab)))
  {
    prev_fanout= COST_MULT(prev_fanout, prev_tab->records_read);
    records=
      COST_ADD(records,
               COST_MULT((double) (tab->get_examined_rows()), prev_fanout));
    prev_tab= tab;
  }
  examined_rows= (double)
    (records > (double) HA_ROWS_MAX ? HA_ROWS_MAX : (ha_rows) records);
  return examined_rows;
}