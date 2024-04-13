Item_bool_rowready_func2::value_depends_on_sql_mode() const
{
  if (compare_collation()->state & MY_CS_NOPAD)
    return Item_func::value_depends_on_sql_mode();
  return ((args[0]->value_depends_on_sql_mode() |
           args[1]->value_depends_on_sql_mode()) &
          Sql_mode_dependency(~0, ~MODE_PAD_CHAR_TO_FULL_LENGTH)).
         soft_to_hard();
}