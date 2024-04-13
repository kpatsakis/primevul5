last_link (struct cpio_file_stat *file_hdr)
{
  return file_hdr->c_nlink == count_defered_links_to_dev_ino (file_hdr) + 1;
}