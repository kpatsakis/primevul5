static bool plugin_load_list(MEM_ROOT *tmp_root, const char *list)
{
  char buffer[FN_REFLEN];
  LEX_STRING name= {buffer, 0}, dl= {NULL, 0}, *str= &name;
  char *p= buffer;
  DBUG_ENTER("plugin_load_list");
  while (list)
  {
    if (p == buffer + sizeof(buffer) - 1)
    {
      sql_print_error("plugin-load parameter too long");
      DBUG_RETURN(TRUE);
    }

    switch ((*(p++)= *(list++))) {
    case '\0':
      list= NULL; /* terminate the loop */
      /* fall through */
    case ';':
#ifndef __WIN__
    case ':':     /* can't use this as delimiter as it may be drive letter */
#endif
      str->str[str->length]= '\0';
      if (str == &name)  // load all plugins in named module
      {
        if (!name.length)
        {
          p--;    /* reset pointer */
          continue;
        }

        dl= name;
        mysql_mutex_lock(&LOCK_plugin);
        free_root(tmp_root, MYF(MY_MARK_BLOCKS_FREE));
        name.str= 0; // load everything
        if (plugin_add(tmp_root, &name, &dl, REPORT_TO_LOG))
          goto error;
      }
      else
      {
        free_root(tmp_root, MYF(MY_MARK_BLOCKS_FREE));
        mysql_mutex_lock(&LOCK_plugin);
        if (plugin_add(tmp_root, &name, &dl, REPORT_TO_LOG))
          goto error;
      }
      mysql_mutex_unlock(&LOCK_plugin);
      name.length= dl.length= 0;
      dl.str= NULL; name.str= p= buffer;
      str= &name;
      continue;
    case '=':
    case '#':
      if (str == &name)
      {
        name.str[name.length]= '\0';
        str= &dl;
        str->str= p;
        continue;
      }
      /* fall through */
    default:
      str->length++;
      continue;
    }
  }
  DBUG_RETURN(FALSE);
error:
  mysql_mutex_unlock(&LOCK_plugin);
  if (name.str)
    sql_print_error("Couldn't load plugin '%s' from '%s'.",
                    name.str, dl.str);
  else
    sql_print_error("Couldn't load plugins from '%s'.", dl.str);
  DBUG_RETURN(TRUE);
}