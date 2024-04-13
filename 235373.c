  bool view_used_tables_processor(void *arg) 
  {
    TABLE_LIST *view_arg= (TABLE_LIST *) arg;
    if (view_arg == view)
      view_arg->view_used_tables|= (*ref)->used_tables();
    return 0;
  }