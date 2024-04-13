cpio_set_c_name (struct cpio_file_stat *file_hdr, char *name)
{
  size_t len = strlen (name) + 1;

  cpio_realloc_c_name (file_hdr, len);
  file_hdr->c_namesize = len;
  memmove (file_hdr->c_name, name, len);
}