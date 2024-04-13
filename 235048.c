inline void Virtual_column_info::print(String* str)
{
  expr->print_for_table_def(str);
}