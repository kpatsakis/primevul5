find_function_def (name)
     const char *name;
{
#if defined (DEBUGGER)
  return ((FUNCTION_DEF *)hash_lookup (name, shell_function_defs));
#else
  return ((FUNCTION_DEF *)0);
#endif
}