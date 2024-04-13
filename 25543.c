uchar *get_plugin_hash_key(const uchar *buff, size_t *length,
                           my_bool not_used __attribute__((unused)))
{
  struct st_plugin_int *plugin= (st_plugin_int *)buff;
  *length= (uint)plugin->name.length;
  return((uchar *)plugin->name.str);
}