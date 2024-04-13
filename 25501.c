my_bool mark_changed(int, const struct my_option *opt, char *)
{
  if (opt->app_type)
  {
    sys_var *var= (sys_var*) opt->app_type;
    var->value_origin= sys_var::CONFIG;
  }
  return 0;
}