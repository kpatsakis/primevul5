read_name_from_file (struct cpio_file_stat *file_hdr, int fd, uintmax_t len)
{
  if (len == 0)
    {
      error (0, 0, _("malformed header: file name of zero length"));
    }
  else
    {
      cpio_realloc_c_name (file_hdr, len);
      tape_buffered_read (file_hdr->c_name, fd, len);
      if (file_hdr->c_name[len-1] != 0)
	{
	  error (0, 0, _("malformed header: file name is not nul-terminated"));
	  /* Skip this file */
	  len = 0;
	}
    }
  file_hdr->c_namesize = len;
}