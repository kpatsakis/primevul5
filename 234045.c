tape_empty_output_buffer (int out_des)
{
  int bytes_written;

#ifdef BROKEN_LONG_TAPE_DRIVER
  static long output_bytes_before_lseek = 0;

  /* Some tape drivers seem to have a signed internal seek pointer and
     they lose if it overflows and becomes negative (e.g. when writing 
     tapes > 2Gb).  Doing an lseek (des, 0, SEEK_SET) seems to reset the 
     seek pointer and prevent it from overflowing.  */
  if (output_is_special
     && ( (output_bytes_before_lseek += output_size) >= 1073741824L) )
    {
      lseek(out_des, 0L, SEEK_SET);
      output_bytes_before_lseek = 0;
    }
#endif

  bytes_written = rmtwrite (out_des, output_buffer, output_size);
  if (bytes_written != output_size)
    {
      int rest_bytes_written;
      int rest_output_size;

      if (output_is_special
	  && (bytes_written >= 0
	      || (bytes_written < 0
		  && (errno == ENOSPC || errno == EIO || errno == ENXIO))))
	{
	  get_next_reel (out_des);
	  if (bytes_written > 0)
	    rest_output_size = output_size - bytes_written;
	  else
	    rest_output_size = output_size;
	  rest_bytes_written = rmtwrite (out_des, output_buffer,
					 rest_output_size);
	  if (rest_bytes_written != rest_output_size)
	    error (PAXEXIT_FAILURE, errno, _("write error"));
	}
      else
	error (PAXEXIT_FAILURE, errno, _("write error"));
    }
  output_bytes += output_size;
  out_buff = output_buffer;
  output_size = 0;
}