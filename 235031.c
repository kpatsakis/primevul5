  void used_tables_and_const_cache_update_and_join(uint argc, Item **argv)
  {
    for (uint i=0 ; i < argc ; i++)
      used_tables_and_const_cache_update_and_join(argv[i]);
  }