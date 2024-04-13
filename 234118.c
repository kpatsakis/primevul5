cpio_realloc_c_name (struct cpio_file_stat *file_hdr, size_t len)
{
  while (file_hdr->c_name_buflen < len)
    file_hdr->c_name = x2realloc (file_hdr->c_name, &file_hdr->c_name_buflen);
}