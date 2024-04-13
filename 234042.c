prepare_append (int out_file_des)
{
  int start_of_header;
  int start_of_block;
  int useful_bytes_in_block;
  char *tmp_buf;

  start_of_header = last_header_start;
  /* Figure out how many bytes we will rewrite, and where they start.  */
  useful_bytes_in_block = start_of_header % io_block_size;
  start_of_block = start_of_header - useful_bytes_in_block;

  if (lseek (out_file_des, start_of_block, SEEK_SET) < 0)
    error (PAXEXIT_FAILURE, errno, _("cannot seek on output"));
  if (useful_bytes_in_block > 0)
    {
      tmp_buf = (char *) xmalloc (useful_bytes_in_block);
      read (out_file_des, tmp_buf, useful_bytes_in_block);
      if (lseek (out_file_des, start_of_block, SEEK_SET) < 0)
	error (PAXEXIT_FAILURE, errno, _("cannot seek on output"));
      /* fix juo -- is this copy_tape_buf_out?  or copy_disk? */
      tape_buffered_write (tmp_buf, out_file_des, useful_bytes_in_block);
      free (tmp_buf);
    }

  /* We are done reading the archive, so clear these since they
     will now be used for reading in files that we are appending
     to the archive.  */
  input_size = 0;
  input_bytes = 0;
  in_buff = input_buffer;
}