Sql_mode_dependency Item_func_like::value_depends_on_sql_mode() const
{
  if (!args[1]->value_depends_on_sql_mode_const_item())
    return Item_func::value_depends_on_sql_mode();
  StringBuffer<64> patternbuf;
  String *pattern= args[1]->val_str_ascii(&patternbuf);
  if (!pattern || !pattern->length())
    return Sql_mode_dependency();                  // Will return NULL or 0
  DBUG_ASSERT(pattern->charset()->mbminlen == 1);
  if (pattern->ptr()[pattern->length() - 1] != '%')
    return Item_func::value_depends_on_sql_mode();
  return ((args[0]->value_depends_on_sql_mode() |
           args[1]->value_depends_on_sql_mode()) &
          Sql_mode_dependency(~0, ~MODE_PAD_CHAR_TO_FULL_LENGTH)).
         soft_to_hard();
}