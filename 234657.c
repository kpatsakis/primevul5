int JOIN_TAB::make_scan_filter()
{
  COND *tmp;
  DBUG_ENTER("make_scan_filter");

  Item *cond= is_inner_table_of_outer_join() ?
                *get_first_inner_table()->on_expr_ref : join->conds;
  
  if (cond &&
      (tmp= make_cond_for_table(join->thd, cond,
                               join->const_table_map | table->map,
			       table->map, -1, FALSE, TRUE)))
  {
     DBUG_EXECUTE("where",print_where(tmp,"cache", QT_ORDINARY););
     if (!(cache_select=
          (SQL_SELECT*) join->thd->memdup((uchar*) select, sizeof(SQL_SELECT))))
	DBUG_RETURN(1);
     cache_select->cond= tmp;
     cache_select->read_tables=join->const_table_map;
  }
  DBUG_RETURN(0);
}