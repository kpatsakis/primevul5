  void set_null_ref_table()
  {
    if (!view->is_inner_table_of_outer_join() ||
        !(null_ref_table= view->get_real_join_table()))
      null_ref_table= NO_NULL_TABLE;
  }