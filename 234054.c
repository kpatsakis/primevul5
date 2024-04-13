writeout_defered_file (struct cpio_file_stat *header, int out_file_des)
{
  int in_file_des;
  struct cpio_file_stat file_hdr;

  file_hdr = *header;


  in_file_des = open (header->c_name,
		      O_RDONLY | O_BINARY, 0);
  if (in_file_des < 0)
    {
      open_error (header->c_name);
      return;
    }

  if (archive_format == arf_crcascii)
    file_hdr.c_chksum = read_for_checksum (in_file_des,
					   file_hdr.c_filesize,
					   header->c_name);

  if (write_out_header (&file_hdr, out_file_des))
    return;
  copy_files_disk_to_tape (in_file_des, out_file_des, file_hdr.c_filesize,
			   header->c_name);
  warn_if_file_changed(header->c_name, file_hdr.c_filesize, file_hdr.c_mtime);

  if (archive_format == arf_tar || archive_format == arf_ustar)
    add_inode (file_hdr.c_ino, file_hdr.c_name, file_hdr.c_dev_maj,
	       file_hdr.c_dev_min);

  tape_pad_output (out_file_des, file_hdr.c_filesize);

  if (reset_time_flag)
    set_file_times (in_file_des, file_hdr.c_name, file_hdr.c_mtime,
		    file_hdr.c_mtime);
  if (close (in_file_des) < 0)
    close_error (header->c_name);
}