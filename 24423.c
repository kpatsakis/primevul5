bool st_select_lex_unit::union_needs_tmp_table()
{
  if (with_element && with_element->is_recursive)
    return true;
  return union_distinct != NULL ||
    global_parameters()->order_list.elements != 0 ||
    thd->lex->sql_command == SQLCOM_INSERT_SELECT ||
    thd->lex->sql_command == SQLCOM_REPLACE_SELECT;
}  