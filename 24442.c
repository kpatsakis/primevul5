LEX::LEX()
  : explain(NULL),
    result(0), arena_for_set_stmt(0), mem_root_for_set_stmt(0),
    option_type(OPT_DEFAULT), context_analysis_only(0), sphead(0),
    is_lex_started(0), limit_rows_examined_cnt(ULONGLONG_MAX)
{

  init_dynamic_array2(&plugins, sizeof(plugin_ref), plugins_static_buffer,
                      INITIAL_LEX_PLUGIN_LIST_SIZE,
                      INITIAL_LEX_PLUGIN_LIST_SIZE, 0);
  reset_query_tables_list(TRUE);
  mi.init();
  init_dynamic_array2(&delete_gtid_domain, sizeof(uint32),
                      gtid_domain_static_buffer,
                      initial_gtid_domain_buffer_size,
                      initial_gtid_domain_buffer_size, 0);
}