read_in_new_ascii (struct cpio_file_stat *file_hdr, int in_des)
{
  struct new_ascii_header ascii_header;

  tape_buffered_read (ascii_header.c_ino, in_des,
		      sizeof ascii_header - sizeof ascii_header.c_magic);

  file_hdr->c_ino = FROM_HEX (ascii_header.c_ino);
  file_hdr->c_mode = FROM_HEX (ascii_header.c_mode);
  file_hdr->c_uid = FROM_HEX (ascii_header.c_uid);
  file_hdr->c_gid = FROM_HEX (ascii_header.c_gid);
  file_hdr->c_nlink = FROM_HEX (ascii_header.c_nlink);
  file_hdr->c_mtime = FROM_HEX (ascii_header.c_mtime);
  file_hdr->c_filesize = FROM_HEX (ascii_header.c_filesize);
  file_hdr->c_dev_maj = FROM_HEX (ascii_header.c_dev_maj);
  file_hdr->c_dev_min = FROM_HEX (ascii_header.c_dev_min);
  file_hdr->c_rdev_maj = FROM_HEX (ascii_header.c_rdev_maj);
  file_hdr->c_rdev_min = FROM_HEX (ascii_header.c_rdev_min);
  file_hdr->c_chksum = FROM_HEX (ascii_header.c_chksum);
  read_name_from_file (file_hdr, in_des, FROM_HEX (ascii_header.c_namesize));

  /* In SVR4 ASCII format, the amount of space allocated for the header
     is rounded up to the next long-word, so we might need to drop
     1-3 bytes.  */
  tape_skip_padding (in_des, file_hdr->c_namesize + 110);
}