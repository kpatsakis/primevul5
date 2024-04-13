static st_plugin_int *plugin_insert_or_reuse(struct st_plugin_int *plugin)
{
  uint i;
  struct st_plugin_int *tmp;
  DBUG_ENTER("plugin_insert_or_reuse");
  for (i= 0; i < plugin_array.elements; i++)
  {
    tmp= *dynamic_element(&plugin_array, i, struct st_plugin_int **);
    if (tmp->state == PLUGIN_IS_FREED)
    {
      memcpy(tmp, plugin, sizeof(struct st_plugin_int));
      DBUG_RETURN(tmp);
    }
  }
  if (insert_dynamic(&plugin_array, (uchar*)&plugin))
    DBUG_RETURN(0);
  tmp= *dynamic_element(&plugin_array, plugin_array.elements - 1,
                        struct st_plugin_int **)=
       (struct st_plugin_int *) memdup_root(&plugin_mem_root, (uchar*)plugin,
                                            sizeof(struct st_plugin_int));
  DBUG_RETURN(tmp);
}