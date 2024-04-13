int _ma_initialize_data_file(MARIA_SHARE *share, File dfile)
{
  if (share->data_file_type == BLOCK_RECORD)
  {
    share->bitmap.block_size= share->base.block_size;
    share->bitmap.file.file = dfile;
    return _ma_bitmap_create_first(share);
  }
  return 0;
}