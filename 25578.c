static inline char plugin_var_bookmark_key(uint flags)
{
  return (flags & PLUGIN_VAR_TYPEMASK) |
         (flags & PLUGIN_VAR_MEMALLOC ? BOOKMARK_MEMALLOC : 0);
}