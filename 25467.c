uchar* sys_var_pluginvar::real_value_ptr(THD *thd, enum_var_type type)
{
  if (type == OPT_DEFAULT)
  {
    switch (plugin_var->flags & PLUGIN_VAR_TYPEMASK) {
    case PLUGIN_VAR_BOOL:
      thd->sys_var_tmp.my_bool_value= (my_bool)option.def_value;
      return (uchar*) &thd->sys_var_tmp.my_bool_value;
    case PLUGIN_VAR_INT:
      thd->sys_var_tmp.int_value= (int)option.def_value;
      return (uchar*) &thd->sys_var_tmp.int_value;
    case PLUGIN_VAR_LONG:
    case PLUGIN_VAR_ENUM:
      thd->sys_var_tmp.long_value= (long)option.def_value;
      return (uchar*) &thd->sys_var_tmp.long_value;
    case PLUGIN_VAR_LONGLONG:
    case PLUGIN_VAR_SET:
      return (uchar*) &option.def_value;
    case PLUGIN_VAR_STR:
      thd->sys_var_tmp.ptr_value= (void*) option.def_value;
      return (uchar*) &thd->sys_var_tmp.ptr_value;
    case PLUGIN_VAR_DOUBLE:
      thd->sys_var_tmp.double_value= getopt_ulonglong2double(option.def_value);
      return (uchar*) &thd->sys_var_tmp.double_value;
    default:
      DBUG_ASSERT(0);
    }
  }

  DBUG_ASSERT(thd || (type == OPT_GLOBAL));
  if (plugin_var->flags & PLUGIN_VAR_THDLOCAL)
  {
    if (type == OPT_GLOBAL)
      thd= NULL;

    return intern_sys_var_ptr(thd, *(int*) (plugin_var+1), false);
  }
  return *(uchar**) (plugin_var+1);
}