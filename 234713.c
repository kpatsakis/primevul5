void JOIN::exec()
{
  DBUG_EXECUTE_IF("show_explain_probe_join_exec_start", 
                  if (dbug_user_var_equals_int(thd, 
                                               "show_explain_probe_select_id", 
                                               select_lex->select_number))
                        dbug_serve_apcs(thd, 1);
                 );
  ANALYZE_START_TRACKING(&explain->time_tracker);
  exec_inner();
  ANALYZE_STOP_TRACKING(&explain->time_tracker);

  DBUG_EXECUTE_IF("show_explain_probe_join_exec_end", 
                  if (dbug_user_var_equals_int(thd, 
                                               "show_explain_probe_select_id", 
                                               select_lex->select_number))
                        dbug_serve_apcs(thd, 1);
                 );
}