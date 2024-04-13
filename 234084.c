tape_buffered_write (char *in_buf, int out_des, off_t num_bytes)
{
  off_t bytes_left = num_bytes;	/* Bytes needing to be copied.  */
  off_t space_left;	/* Room left in output buffer.  */

  while (bytes_left > 0)
    {
      space_left = io_block_size - output_size;
      if (space_left == 0)
	tape_empty_output_buffer (out_des);
      else
	{
	  if (bytes_left < space_left)
	    space_left = bytes_left;
	  memcpy (out_buff, in_buf, (unsigned) space_left);
	  out_buff += space_left;
	  output_size += space_left;
	  in_buf += space_left;
	  bytes_left -= space_left;
	}
    }
}