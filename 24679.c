  bool pushable_cond_checker_for_derived(uchar *arg)
  {
    return excl_dep_on_table(*((table_map *)arg));
  }