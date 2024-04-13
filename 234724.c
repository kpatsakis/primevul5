void JOIN::handle_implicit_grouping_with_window_funcs()
{
  if (select_lex->have_window_funcs() && send_row_on_empty_set())
  {
    const_tables= top_join_tab_count= table_count= 0;
  }
}