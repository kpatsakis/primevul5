static int check_func_enum(THD *thd, struct st_mysql_sys_var *var,
                           void *save, st_mysql_value *value)
{
  char buff[STRING_BUFFER_USUAL_SIZE];
  const char *str;
  TYPELIB *typelib;
  long long tmp;
  long result;
  int length;

  if (var->flags & PLUGIN_VAR_THDLOCAL)
    typelib= ((thdvar_enum_t*) var)->typelib;
  else
    typelib= ((sysvar_enum_t*) var)->typelib;

  if (value->value_type(value) == MYSQL_VALUE_TYPE_STRING)
  {
    length= sizeof(buff);
    if (!(str= value->val_str(value, buff, &length)))
      goto err;
    if ((result= (long)find_type(typelib, str, length, 0) - 1) < 0)
      goto err;
  }
  else
  {
    if (value->val_int(value, &tmp))
      goto err;
    if (tmp < 0 || tmp >= typelib->count)
      goto err;
    result= (long) tmp;
  }
  *(long*)save= result;
  return 0;
err:
  return 1;
}