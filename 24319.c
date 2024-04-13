static int rr_unpack_from_tempfile(READ_RECORD *info)
{
  if (my_b_read(info->io_cache, info->rec_buf, info->ref_length))
    return -1;
  (*info->unpack)(info->addon_field, info->rec_buf,
                  info->rec_buf + info->ref_length);

  return 0;
}