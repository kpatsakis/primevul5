static struct st_plugin_int *plugin_find_internal(const LEX_STRING *name, int type)
{
  uint i;
  DBUG_ENTER("plugin_find_internal");
  if (! initialized)
    DBUG_RETURN(0);

  mysql_mutex_assert_owner(&LOCK_plugin);

  if (type == MYSQL_ANY_PLUGIN)
  {
    for (i= 0; i < MYSQL_MAX_PLUGIN_TYPE_NUM; i++)
    {
      struct st_plugin_int *plugin= (st_plugin_int *)
        my_hash_search(&plugin_hash[i], (const uchar *)name->str, name->length);
      if (plugin)
        DBUG_RETURN(plugin);
    }
  }
  else
    DBUG_RETURN((st_plugin_int *)
        my_hash_search(&plugin_hash[type], (const uchar *)name->str,
                       name->length));
  DBUG_RETURN(0);
}