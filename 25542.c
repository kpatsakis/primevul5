bool plugin_is_forced(struct st_plugin_int *p)
{
  return p->load_option == PLUGIN_FORCE ||
         p->load_option == PLUGIN_FORCE_PLUS_PERMANENT;
}