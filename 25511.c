bool plugin_is_ready(const LEX_STRING *name, int type)
{
  bool rc= FALSE;
  if (plugin_status(name, type) == SHOW_OPTION_YES)
    rc= TRUE;
  return rc;
}