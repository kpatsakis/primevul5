bool dbug_user_var_equals_int(THD *thd, const char *name, int value)
{
  user_var_entry *var;
  LEX_STRING varname= {(char*)name, strlen(name)};
  if ((var= get_variable(&thd->user_vars, varname, FALSE)))
  {
    bool null_value;
    longlong var_value= var->val_int(&null_value);
    if (!null_value && var_value == value)
      return TRUE;
  }
  return FALSE;
}