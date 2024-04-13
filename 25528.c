static int check_func_longlong(THD *thd, struct st_mysql_sys_var *var,
                               void *save, st_mysql_value *value)
{
  my_bool fixed1, fixed2;
  long long orig, val;
  struct my_option options;
  value->val_int(value, &orig);
  val= orig;
  plugin_opt_set_limits(&options, var);

  if (var->flags & PLUGIN_VAR_UNSIGNED)
  {
    if ((fixed1= (!value->is_unsigned(value) && val < 0)))
      val=0;
    *(ulonglong *)save= getopt_ull_limit_value((ulonglong) val, &options,
                                               &fixed2);
  }
  else
  {
    if ((fixed1= (value->is_unsigned(value) && val < 0)))
      val=LONGLONG_MAX;
    *(longlong *)save= getopt_ll_limit_value(val, &options, &fixed2);
  }

  return throw_bounds_warning(thd, var->name, fixed1 || fixed2,
                              value->is_unsigned(value), (longlong) orig);
}