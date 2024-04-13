copy_files_tape_to_disk (int in_des, int out_des, off_t num_bytes)
{
  off_t size;
  off_t k;

  while (num_bytes > 0)
    {
      if (input_size == 0)
	tape_fill_input_buffer (in_des, io_block_size);
      size = (input_size < num_bytes) ? input_size : num_bytes;
      if (crc_i_flag)
	{
	  for (k = 0; k < size; ++k)
	    crc += in_buff[k] & 0xff;
	}
      disk_buffered_write (in_buff, out_des, size);
      num_bytes -= size;
      input_size -= size;
      in_buff += size;
    }
}