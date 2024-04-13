double ha_maria::scan_time()
{
  if (file->s->data_file_type == BLOCK_RECORD)
    return (ulonglong2double(stats.data_file_length - file->s->block_size) /
            file->s->block_size) + 2;
  return handler::scan_time();
}