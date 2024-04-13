static const void *var_def_ptr(st_mysql_sys_var *pv)
{
    switch (pv->flags & (PLUGIN_VAR_TYPEMASK | PLUGIN_VAR_THDLOCAL)) {
    case PLUGIN_VAR_INT:
      return &((sysvar_uint_t*) pv)->def_val;
    case PLUGIN_VAR_LONG:
      return &((sysvar_ulong_t*) pv)->def_val;
    case PLUGIN_VAR_LONGLONG:
      return &((sysvar_ulonglong_t*) pv)->def_val;
    case PLUGIN_VAR_ENUM:
      return &((sysvar_enum_t*) pv)->def_val;
    case PLUGIN_VAR_SET:
      return &((sysvar_set_t*) pv)->def_val;
    case PLUGIN_VAR_BOOL:
      return &((sysvar_bool_t*) pv)->def_val;
    case PLUGIN_VAR_STR:
      return &((sysvar_str_t*) pv)->def_val;
    case PLUGIN_VAR_DOUBLE:
      return &((sysvar_double_t*) pv)->def_val;
    case PLUGIN_VAR_INT | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_uint_t*) pv)->def_val;
    case PLUGIN_VAR_LONG | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_ulong_t*) pv)->def_val;
    case PLUGIN_VAR_LONGLONG | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_ulonglong_t*) pv)->def_val;
    case PLUGIN_VAR_ENUM | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_enum_t*) pv)->def_val;
    case PLUGIN_VAR_SET | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_set_t*) pv)->def_val;
    case PLUGIN_VAR_BOOL | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_bool_t*) pv)->def_val;
    case PLUGIN_VAR_STR | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_str_t*) pv)->def_val;
    case PLUGIN_VAR_DOUBLE | PLUGIN_VAR_THDLOCAL:
      return &((thdvar_double_t*) pv)->def_val;
    default:
      DBUG_ASSERT(0);
      return NULL;
    }
}