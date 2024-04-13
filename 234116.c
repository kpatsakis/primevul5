tape_clear_rest_of_block (int out_file_des)
{
  write_nuls_to_file (io_block_size - output_size, out_file_des, 
                      tape_buffered_write);
}