cpio_file_stat_init (struct cpio_file_stat *file_hdr)
{
  memset (file_hdr, 0, sizeof (*file_hdr));
}