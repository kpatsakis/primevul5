static int check_func_bool(THD *thd, struct st_mysql_sys_var *var,
                           void *save, st_mysql_value *value)
{
  char buff[STRING_BUFFER_USUAL_SIZE];
  const char *str;
  int result, length;
  long long tmp;

  if (value->value_type(value) == MYSQL_VALUE_TYPE_STRING)
  {
    length= sizeof(buff);
    if (!(str= value->val_str(value, buff, &length)) ||
        (result= find_type(&bool_typelib, str, length, 1)-1) < 0)
      goto err;
  }
  else
  {
    if (value->val_int(value, &tmp) < 0)
      goto err;
    if (tmp != 0 && tmp != 1)
      goto err;
    result= (int) tmp;
  }
  *(my_bool *) save= result ? 1 : 0;
  return 0;
err:
  return 1;
}