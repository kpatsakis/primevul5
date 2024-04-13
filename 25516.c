static SHOW_TYPE pluginvar_show_type(const st_mysql_sys_var *plugin_var)
{
  switch (plugin_var->flags & (PLUGIN_VAR_TYPEMASK | PLUGIN_VAR_UNSIGNED)) {
  case PLUGIN_VAR_BOOL:
    return SHOW_MY_BOOL;
  case PLUGIN_VAR_INT:
    return SHOW_SINT;
  case PLUGIN_VAR_INT | PLUGIN_VAR_UNSIGNED:
    return SHOW_UINT;
  case PLUGIN_VAR_LONG:
    return SHOW_SLONG;
  case PLUGIN_VAR_LONG | PLUGIN_VAR_UNSIGNED:
    return SHOW_ULONG;
  case PLUGIN_VAR_LONGLONG:
    return SHOW_SLONGLONG;
  case PLUGIN_VAR_LONGLONG | PLUGIN_VAR_UNSIGNED:
    return SHOW_ULONGLONG;
  case PLUGIN_VAR_STR:
    return SHOW_CHAR_PTR;
  case PLUGIN_VAR_ENUM:
  case PLUGIN_VAR_SET:
    return SHOW_CHAR;
  case PLUGIN_VAR_DOUBLE:
    return SHOW_DOUBLE;
  default:
    DBUG_ASSERT(0);
    return SHOW_UNDEF;
  }
}