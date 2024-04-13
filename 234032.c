write_out_binary_header (dev_t rdev,
			 struct cpio_file_stat *file_hdr, int out_des)
{
  struct old_cpio_header short_hdr;

  short_hdr.c_magic = 070707;
  short_hdr.c_dev = makedev (file_hdr->c_dev_maj, file_hdr->c_dev_min);

  if ((warn_option & CPIO_WARN_TRUNCATE) && (file_hdr->c_ino >> 16) != 0)
    error (0, 0, _("%s: truncating inode number"), file_hdr->c_name);

  short_hdr.c_ino = file_hdr->c_ino & 0xFFFF;
  if (short_hdr.c_ino != file_hdr->c_ino)
    field_width_warning (file_hdr->c_name, _("inode number"));
  
  short_hdr.c_mode = file_hdr->c_mode & 0xFFFF;
  if (short_hdr.c_mode != file_hdr->c_mode)
    field_width_warning (file_hdr->c_name, _("file mode"));
  
  short_hdr.c_uid = file_hdr->c_uid & 0xFFFF;
  if (short_hdr.c_uid != file_hdr->c_uid)
    field_width_warning (file_hdr->c_name, _("uid"));
  
  short_hdr.c_gid = file_hdr->c_gid & 0xFFFF;
  if (short_hdr.c_gid != file_hdr->c_gid)
    field_width_warning (file_hdr->c_name, _("gid"));
  
  short_hdr.c_nlink = file_hdr->c_nlink & 0xFFFF;
  if (short_hdr.c_nlink != file_hdr->c_nlink)
    field_width_warning (file_hdr->c_name, _("number of links"));
		      
  short_hdr.c_rdev = rdev;
  short_hdr.c_mtimes[0] = file_hdr->c_mtime >> 16;
  short_hdr.c_mtimes[1] = file_hdr->c_mtime & 0xFFFF;

  short_hdr.c_namesize = file_hdr->c_namesize & 0xFFFF;
  if (short_hdr.c_namesize != file_hdr->c_namesize)
    {
      char maxbuf[UINTMAX_STRSIZE_BOUND + 1];
      error (0, 0, _("%s: value %s %s out of allowed range 0..%u"),
	     file_hdr->c_name, _("name size"),
	     STRINGIFY_BIGINT (file_hdr->c_namesize, maxbuf), 0xFFFFu);
      return 1;
    }
		      
  short_hdr.c_filesizes[0] = file_hdr->c_filesize >> 16;
  short_hdr.c_filesizes[1] = file_hdr->c_filesize & 0xFFFF;

  if (((off_t)short_hdr.c_filesizes[0] << 16) + short_hdr.c_filesizes[1]
       != file_hdr->c_filesize)
    {
      char maxbuf[UINTMAX_STRSIZE_BOUND + 1];
      error (0, 0, _("%s: value %s %s out of allowed range 0..%lu"),
	     file_hdr->c_name, _("file size"),
	     STRINGIFY_BIGINT (file_hdr->c_namesize, maxbuf), 0xFFFFFFFFlu);
      return 1;
    }
		      
  /* Output the file header.  */
  tape_buffered_write ((char *) &short_hdr, out_des, 26);

  /* Write file name to output.  */
  tape_buffered_write (file_hdr->c_name, out_des, file_hdr->c_namesize);

  tape_pad_output (out_des, file_hdr->c_namesize + 26);
  return 0;
}