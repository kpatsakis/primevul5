tape_buffered_peek (char *peek_buf, int in_des, int num_bytes)
{
  long tmp_input_size;
  long got_bytes;
  char *append_buf;

#ifdef BROKEN_LONG_TAPE_DRIVER
  /* Some tape drivers seem to have a signed internal seek pointer and
     they lose if it overflows and becomes negative (e.g. when writing 
     tapes > 4Gb).  Doing an lseek (des, 0, SEEK_SET) seems to reset the 
     seek pointer and prevent it from overflowing.  */
  if (input_is_special
      && ( (input_bytes_before_lseek += num_bytes) >= 1073741824L) )
    {
      lseek(in_des, 0L, SEEK_SET);
      input_bytes_before_lseek = 0;
    }
#endif

  while (input_size < num_bytes)
    {
      append_buf = in_buff + input_size;
      if ( (append_buf - input_buffer) >= input_buffer_size)
	{
	  /* We can keep up to 2 "blocks" (either the physical block size
	     or 512 bytes(the size of a tar record), which ever is
	     larger) in the input buffer when we are peeking.  We
	     assume that our caller will never be interested in peeking
	     ahead at more than 512 bytes, so we know that by the time
	     we need a 3rd "block" in the buffer we can throw away the
	     first block to make room.  */
	  int half;
	  half = input_buffer_size / 2;
	  memmove (input_buffer, input_buffer + half, half);
	  in_buff = in_buff - half;
	  append_buf = append_buf - half;
	}
      tmp_input_size = rmtread (in_des, append_buf, io_block_size);
      if (tmp_input_size == 0)
	{
	  if (input_is_special)
	    {
	      get_next_reel (in_des);
	      tmp_input_size = rmtread (in_des, append_buf, io_block_size);
	    }
	  else
	    break;
	}
      if (tmp_input_size < 0)
	error (PAXEXIT_FAILURE, errno, _("read error"));
      input_bytes += tmp_input_size;
      input_size += tmp_input_size;
    }
  if (num_bytes <= input_size)
    got_bytes = num_bytes;
  else
    got_bytes = input_size;
  memcpy (peek_buf, in_buff, (unsigned) got_bytes);
  return got_bytes;
}