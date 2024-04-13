  table_map view_used_tables(TABLE_LIST *view)
  {
    view->view_used_tables= 0;
    walk(&Item::view_used_tables_processor, 0, view);
    return view->view_used_tables;
  }