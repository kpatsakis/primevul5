sparse_extract_file (int fd, struct tar_stat_info *st, off_t *size)
{
  bool rc = true;
  struct tar_sparse_file file;
  size_t i;

  if (!tar_sparse_init (&file))
    return dump_status_not_implemented;

  file.stat_info = st;
  file.fd = fd;
  file.seekable = lseek (fd, 0, SEEK_SET) == 0;
  file.offset = 0;

  rc = tar_sparse_decode_header (&file);
  for (i = 0; rc && i < file.stat_info->sparse_map_avail; i++)
    rc = tar_sparse_extract_region (&file, i);
  *size = file.stat_info->archive_file_size - file.dumped_size;
  return (tar_sparse_done (&file) && rc) ? dump_status_ok : dump_status_short;
}
