tape_toss_input (int in_des, off_t num_bytes)
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

      if (crc_i_flag && only_verify_crc_flag)
	{
 	  int k;
	  for (k = 0; k < space_left; ++k)
	    crc += in_buff[k] & 0xff;
	}

      in_buff += space_left;
      input_size -= space_left;
      bytes_left -= space_left;
    }
}