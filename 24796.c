  bool pushable_cond_checker_for_subquery(uchar *arg)
  {
    return excl_dep_on_in_subq_left_part((Item_in_subselect *)arg);
  }