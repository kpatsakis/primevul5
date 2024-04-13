uchar* sys_var_pluginvar::do_value_ptr(THD *thd, enum_var_type type,
                                       const LEX_STRING *base)
{
  uchar* result;

  result= real_value_ptr(thd, type);

  if ((plugin_var->flags & PLUGIN_VAR_TYPEMASK) == PLUGIN_VAR_ENUM)
    result= (uchar*) get_type(plugin_var_typelib(), *(ulong*)result);
  else if ((plugin_var->flags & PLUGIN_VAR_TYPEMASK) == PLUGIN_VAR_SET)
    result= (uchar*) set_to_string(thd, 0, *(ulonglong*) result,
                                   plugin_var_typelib()->type_names);
  return result;
}