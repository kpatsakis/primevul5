  ~Name_resolution_context_backup()
  {
    ctx_state.restore_state(&ctx, &table_list);
    table_list.map= save_map;
  }