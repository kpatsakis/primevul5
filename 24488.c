enum row_type ha_maria::get_row_type() const
{
  switch (file->s->data_file_type) {
  case STATIC_RECORD:     return ROW_TYPE_FIXED;
  case DYNAMIC_RECORD:    return ROW_TYPE_DYNAMIC;
  case BLOCK_RECORD:      return ROW_TYPE_PAGE;
  case COMPRESSED_RECORD: return ROW_TYPE_COMPRESSED;
  default:                return ROW_TYPE_NOT_USED;
  }
}