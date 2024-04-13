cpio_file_stat_free (struct cpio_file_stat *file_hdr)
{
  free (file_hdr->c_name);
  cpio_file_stat_init (file_hdr);
}