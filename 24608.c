  Name_resolution_context_backup(Name_resolution_context &_ctx, TABLE_LIST &_table_list)
    : ctx(_ctx), table_list(_table_list), save_map(_table_list.map)
  {
    ctx_state.save_state(&ctx, &table_list);
    ctx.table_list= &table_list;
    ctx.first_name_resolution_table= &table_list;
  }