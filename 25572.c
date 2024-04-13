SHOW_COMP_OPTION plugin_status(const char *name, size_t len, int type)
{
  LEX_STRING plugin_name= { (char *) name, len };
  return plugin_status(&plugin_name, type);
}