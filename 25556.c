static struct st_plugin_dl *plugin_dl_find(const LEX_STRING *dl)
{
  uint i;
  struct st_plugin_dl *tmp;
  DBUG_ENTER("plugin_dl_find");
  for (i= 0; i < plugin_dl_array.elements; i++)
  {
    tmp= *dynamic_element(&plugin_dl_array, i, struct st_plugin_dl **);
    if (tmp->ref_count &&
        ! my_strnncoll(files_charset_info,
                       (const uchar *)dl->str, dl->length,
                       (const uchar *)tmp->dl.str, tmp->dl.length))
      DBUG_RETURN(tmp);
  }
  DBUG_RETURN(0);
}