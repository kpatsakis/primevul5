static size_t var_storage_size(int flags)
{
  switch (flags & PLUGIN_VAR_TYPEMASK) {
  case PLUGIN_VAR_BOOL:         return sizeof(my_bool);
  case PLUGIN_VAR_INT:          return sizeof(int);
  case PLUGIN_VAR_LONG:         return sizeof(long);
  case PLUGIN_VAR_ENUM:         return sizeof(long);
  case PLUGIN_VAR_LONGLONG:     return sizeof(ulonglong);
  case PLUGIN_VAR_SET:          return sizeof(ulonglong);
  case PLUGIN_VAR_STR:          return sizeof(char*);
  case PLUGIN_VAR_DOUBLE:       return sizeof(double);
  default: DBUG_ASSERT(0);      return 0;
  }
}