  bool pushable_equality_checker_for_derived(uchar *arg)
  {
    return (used_tables() == *((table_map *)arg));
  }