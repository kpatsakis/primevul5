static st_plugin_dl *plugin_dl_insert_or_reuse(struct st_plugin_dl *plugin_dl)
{
  uint i;
  struct st_plugin_dl *tmp;
  DBUG_ENTER("plugin_dl_insert_or_reuse");
  for (i= 0; i < plugin_dl_array.elements; i++)
  {
    tmp= *dynamic_element(&plugin_dl_array, i, struct st_plugin_dl **);
    if (! tmp->ref_count)
    {
      memcpy(tmp, plugin_dl, sizeof(struct st_plugin_dl));
      DBUG_RETURN(tmp);
    }
  }
  if (insert_dynamic(&plugin_dl_array, (uchar*)&plugin_dl))
    DBUG_RETURN(0);
  tmp= *dynamic_element(&plugin_dl_array, plugin_dl_array.elements - 1,
                        struct st_plugin_dl **)=
      (struct st_plugin_dl *) memdup_root(&plugin_mem_root, (uchar*)plugin_dl,
                                           sizeof(struct st_plugin_dl));
  DBUG_RETURN(tmp);
}