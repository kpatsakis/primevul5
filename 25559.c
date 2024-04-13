static int check_func_double(THD *thd, struct st_mysql_sys_var *var,
                             void *save, st_mysql_value *value)
{
  double v;
  my_bool fixed;
  struct my_option option;

  value->val_real(value, &v);
  plugin_opt_set_limits(&option, var);
  *(double *) save= getopt_double_limit_value(v, &option, &fixed);

  return throw_bounds_warning(thd, var->name, fixed, v);
}