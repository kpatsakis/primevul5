tape_buffered_read (char *in_buf, int in_des, off_t num_bytes)
{
  off_t bytes_left = num_bytes;	/* Bytes needing to be copied.  */
  off_t space_left;	/* Bytes to copy from input buffer.  */

  while (bytes_left > 0)
    {
      if (input_size == 0)
	tape_fill_input_buffer (in_des, io_block_size);
      if (bytes_left < input_size)
	space_left = bytes_left;
      else
	space_left = input_size;
      memcpy (in_buf, in_buff, (unsigned) space_left);
      in_buff += space_left;
      in_buf += space_left;
      input_size -= space_left;
      bytes_left -= space_left;
    }
}