write_out_old_ascii_header (dev_t dev, dev_t rdev,
			    struct cpio_file_stat *file_hdr, int out_des)
{
  char ascii_header[76];
  char *p = ascii_header;
  
  to_ascii (p, file_hdr->c_magic, 6, LG_8, false);
  p += 6;
  to_ascii_or_warn (p, dev, 6, LG_8, file_hdr->c_name, _("device number"));
  p += 6;
  to_ascii_or_warn (p, file_hdr->c_ino, 6, LG_8, file_hdr->c_name,
		    _("inode number"));
  p += 6;
  to_ascii_or_warn (p, file_hdr->c_mode, 6, LG_8, file_hdr->c_name,
		    _("file mode"));
  p += 6;
  to_ascii_or_warn (p, file_hdr->c_uid, 6, LG_8, file_hdr->c_name, _("uid"));
  p += 6;
  to_ascii_or_warn (p, file_hdr->c_gid, 6, LG_8, file_hdr->c_name, _("gid"));
  p += 6;
  to_ascii_or_warn (p, file_hdr->c_nlink, 6, LG_8, file_hdr->c_name,
		    _("number of links"));
  p += 6;
  to_ascii_or_warn (p, rdev, 6, LG_8, file_hdr->c_name, _("rdev"));
  p += 6;
  to_ascii_or_warn (p, file_hdr->c_mtime, 11, LG_8, file_hdr->c_name,
		    _("modification time"));
  p += 11;
  if (to_ascii_or_error (p, file_hdr->c_namesize, 6, LG_8, file_hdr->c_name,
			 _("name size")))
    return 1;
  p += 6;
  if (to_ascii_or_error (p, file_hdr->c_filesize, 11, LG_8, file_hdr->c_name,
			 _("file size")))
    return 1;

  tape_buffered_write (ascii_header, out_des, sizeof ascii_header);

  /* Write file name to output.  */
  tape_buffered_write (file_hdr->c_name, out_des, file_hdr->c_namesize);
  return 0;
}