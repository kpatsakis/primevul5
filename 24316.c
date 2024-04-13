static int rr_unpack_from_buffer(READ_RECORD *info)
{
  if (info->cache_pos == info->cache_end)
    return -1;                      /* End of buffer */
  (*info->unpack)(info->addon_field, info->cache_pos,
                  info->cache_end);
  info->cache_pos+= info->ref_length;
  return 0;
}