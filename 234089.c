disk_empty_output_buffer (int out_des, bool flush)
{
  ssize_t bytes_written;

  if (swapping_halfwords || swapping_bytes)
    {
      if (swapping_halfwords)
	{
	  int complete_words;
	  complete_words = output_size / 4;
	  swahw_array (output_buffer, complete_words);
	  if (swapping_bytes)
	    swab_array (output_buffer, 2 * complete_words);
	}
      else
	{
	  int complete_halfwords;
	  complete_halfwords = output_size /2;
	  swab_array (output_buffer, complete_halfwords);
	}
    }

  if (sparse_flag)
    bytes_written = sparse_write (out_des, output_buffer, output_size, flush);
  else
    bytes_written = write (out_des, output_buffer, output_size);

  if (bytes_written != output_size)
    {
      if (bytes_written == -1)
	error (PAXEXIT_FAILURE, errno, _("write error"));
      else
	error (PAXEXIT_FAILURE, 0, _("write error: partial write"));
    }
  output_bytes += output_size;
  out_buff = output_buffer;
  output_size = 0;
}