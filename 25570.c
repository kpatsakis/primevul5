bool sys_var_pluginvar::session_is_default(THD *thd)
{
  uchar *value= plugin_var->flags & PLUGIN_VAR_THDLOCAL
                ? intern_sys_var_ptr(thd, *(int*) (plugin_var+1), true)
                : *(uchar**) (plugin_var+1);

    real_value_ptr(thd, OPT_SESSION);

  switch (plugin_var->flags & PLUGIN_VAR_TYPEMASK) {
  case PLUGIN_VAR_BOOL:
    return option.def_value == *(my_bool*)value;
  case PLUGIN_VAR_INT:
    return option.def_value == *(int*)value;
  case PLUGIN_VAR_LONG:
  case PLUGIN_VAR_ENUM:
    return option.def_value == *(long*)value;
  case PLUGIN_VAR_LONGLONG:
  case PLUGIN_VAR_SET:
    return option.def_value == *(longlong*)value;
  case PLUGIN_VAR_STR:
    {
      const char *a=(char*)option.def_value;
      const char *b=(char*)value;
      return (!a && !b) || (a && b && strcmp(a,b));
    }
  case PLUGIN_VAR_DOUBLE:
    return getopt_ulonglong2double(option.def_value) == *(double*)value;
  }
  DBUG_ASSERT(0);
  return 0;
}