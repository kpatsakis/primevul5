copy_files_disk_to_tape (int in_des, int out_des, off_t num_bytes,
			 char *filename)
{
  off_t size;
  off_t k;
  int rc;
  off_t original_num_bytes;

  original_num_bytes = num_bytes;

  while (num_bytes > 0)
    {
      if (input_size == 0)
	if ((rc = disk_fill_input_buffer (in_des,
					  num_bytes < DISK_IO_BLOCK_SIZE ?
					  num_bytes : DISK_IO_BLOCK_SIZE)))
	  {
	    if (rc > 0)
	      {
		  char buf[UINTMAX_STRSIZE_BOUND];
		  error (0, 0,
			 ngettext ("File %s shrunk by %s byte, padding with zeros",
				   "File %s shrunk by %s bytes, padding with zeros",
				   num_bytes),
			 filename,  STRINGIFY_BIGINT (num_bytes, buf));
	      }
	    else
	      error (0, 0,
		     _("Read error at byte %lld in file %s, padding with zeros"),
		     (long long) (original_num_bytes - num_bytes), filename);
	    write_nuls_to_file (num_bytes, out_des, tape_buffered_write);
	    break;
	  }
      size = (input_size < num_bytes) ? input_size : num_bytes;
      if (crc_i_flag)
	{
	  for (k = 0; k < size; ++k)
	    crc += in_buff[k] & 0xff;
	}
      tape_buffered_write (in_buff, out_des, size);
      num_bytes -= size;
      input_size -= size;
      in_buff += size;
    }
}