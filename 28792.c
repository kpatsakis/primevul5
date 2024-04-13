sparse_scan_file_seek (struct tar_sparse_file *file)
{
  struct tar_stat_info *st = file->stat_info;
  int fd = file->fd;
  struct sp_array sp = {0, 0};
  off_t offset = 0;
  off_t data_offset;
  off_t hole_offset;

  st->archive_file_size = 0;

  for (;;)
    {
      /* locate first chunk of data */
      data_offset = lseek (fd, offset, SEEK_DATA);

      if (data_offset == (off_t)-1)
        /* ENXIO == EOF; error otherwise */
        {
          if (errno == ENXIO)
            {
              /* file ends with hole, add one more empty chunk of data */
              sp.numbytes = 0;
              sp.offset = st->stat.st_size;
              sparse_add_map (st, &sp);
              return true;
            }
          return false;
        }

      hole_offset = lseek (fd, data_offset, SEEK_HOLE);

      /* according to specs, if FS does not fully support
	 SEEK_DATA/SEEK_HOLE it may just implement kind of "wrapper" around
	 classic lseek() call.  We must detect it here and try to use other
	 hole-detection methods. */
      if (offset == 0 /* first loop */
          && data_offset == 0
          && hole_offset == st->stat.st_size)
        {
          lseek (fd, 0, SEEK_SET);
          return false;
        }

      sp.offset = data_offset;
      sp.numbytes = hole_offset - data_offset;
      sparse_add_map (st, &sp);

      st->archive_file_size += sp.numbytes;
      offset = hole_offset;
    }

  return true;
}
