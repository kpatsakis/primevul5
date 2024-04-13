sparse_scan_file_raw (struct tar_sparse_file *file)
{
  struct tar_stat_info *st = file->stat_info;
  int fd = file->fd;
  char buffer[BLOCKSIZE];
  size_t count = 0;
  off_t offset = 0;
  struct sp_array sp = {0, 0};

  st->archive_file_size = 0;

  if (!tar_sparse_scan (file, scan_begin, NULL))
    return false;

  while ((count = blocking_read (fd, buffer, sizeof buffer)) != 0
         && count != SAFE_READ_ERROR)
    {
      /* Analyze the block.  */
      if (zero_block_p (buffer, count))
        {
          if (sp.numbytes)
            {
              sparse_add_map (st, &sp);
              sp.numbytes = 0;
              if (!tar_sparse_scan (file, scan_block, NULL))
                return false;
            }
        }
      else
        {
          if (sp.numbytes == 0)
            sp.offset = offset;
          sp.numbytes += count;
          st->archive_file_size += count;
          if (!tar_sparse_scan (file, scan_block, buffer))
            return false;
        }

      offset += count;
    }

  /* save one more sparse segment of length 0 to indicate that
     the file ends with a hole */
  if (sp.numbytes == 0)
    sp.offset = offset;

  sparse_add_map (st, &sp);
  st->archive_file_size += count;
  return tar_sparse_scan (file, scan_end, NULL);
}
