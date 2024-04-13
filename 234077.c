disk_fill_input_buffer (int in_des, off_t num_bytes)
{
  in_buff = input_buffer;
  num_bytes = (num_bytes < DISK_IO_BLOCK_SIZE) ? num_bytes : DISK_IO_BLOCK_SIZE;
  input_size = read (in_des, input_buffer, num_bytes);
  if (input_size == SAFE_READ_ERROR)
    {
      input_size = 0;
      return (-1);
    }
  else if (input_size == 0)
    return (1);
  input_bytes += input_size;
  return (0);
}