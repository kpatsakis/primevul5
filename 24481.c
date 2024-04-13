static enum data_file_type maria_row_type(HA_CREATE_INFO *info)
{
  if (info->transactional == HA_CHOICE_YES)
    return BLOCK_RECORD;
  switch (info->row_type) {
  case ROW_TYPE_FIXED:   return STATIC_RECORD;
  case ROW_TYPE_DYNAMIC: return DYNAMIC_RECORD;
  default:               return BLOCK_RECORD;
  }
}