static st_bookmark *find_bookmark(const char *plugin, const char *name,
                                  int flags)
{
  st_bookmark *result= NULL;
  uint namelen, length, pluginlen= 0;
  char *varname, *p;

  if (!(flags & PLUGIN_VAR_THDLOCAL))
    return NULL;

  namelen= strlen(name);
  if (plugin)
    pluginlen= strlen(plugin) + 1;
  length= namelen + pluginlen + 2;
  varname= (char*) my_alloca(length);

  if (plugin)
  {
    strxmov(varname + 1, plugin, "_", name, NullS);
    for (p= varname + 1; *p; p++)
      if (*p == '-')
        *p= '_';
  }
  else
    memcpy(varname + 1, name, namelen + 1);

  varname[0]= plugin_var_bookmark_key(flags);

  result= (st_bookmark*) my_hash_search(&bookmark_hash,
                                        (const uchar*) varname, length - 1);

  my_afree(varname);
  return result;
}