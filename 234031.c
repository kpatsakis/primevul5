tape_fill_input_buffer (int in_des, int num_bytes)
{
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
  in_buff = input_buffer;
  num_bytes = (num_bytes < io_block_size) ? num_bytes : io_block_size;
  input_size = rmtread (in_des, input_buffer, num_bytes);
  if (input_size == 0 && input_is_special)
    {
      get_next_reel (in_des);
      input_size = rmtread (in_des, input_buffer, num_bytes);
    }
  if (input_size == SAFE_READ_ERROR)
    error (PAXEXIT_FAILURE, errno, _("read error"));
  if (input_size == 0)
    error (PAXEXIT_FAILURE, 0, _("premature end of file"));
  input_bytes += input_size;
}