update_export_env_inplace (env_prefix, preflen, value)
     char *env_prefix;
     int preflen;
     char *value;
{
  char *evar;

  evar = (char *)xmalloc (STRLEN (value) + preflen + 1);
  strcpy (evar, env_prefix);
  if (value)
    strcpy (evar + preflen, value);
  export_env = add_or_supercede_exported_var (evar, 0);
}