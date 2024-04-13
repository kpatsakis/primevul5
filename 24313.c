static ulonglong update_tot_length(ulonglong tot_length, ulonglong max_rows, uint length)
{
  ulonglong tot_length_part;

  if (tot_length == ULONGLONG_MAX)
    return ULONGLONG_MAX;

  tot_length_part= (max_rows/(ulong) ((maria_block_size -
                    MAX_KEYPAGE_HEADER_SIZE - KEYPAGE_CHECKSUM_SIZE)/
                    (length*2)));
  if (tot_length_part >=  ULONGLONG_MAX / maria_block_size)
    return ULONGLONG_MAX;

  if (tot_length > ULONGLONG_MAX - tot_length_part * maria_block_size)
    return ULONGLONG_MAX;

  return tot_length + tot_length_part * maria_block_size;
}