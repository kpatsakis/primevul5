void free_cache(READ_RECORD *info)
{
  if (info->cache)
  {
    my_free_lock(info->cache);
    info->cache=0;
  }
}