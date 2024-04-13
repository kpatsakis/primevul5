int ha_maria::update_row(const uchar * old_data, const uchar * new_data)
{
  CHECK_UNTIL_WE_FULLY_IMPLEMENTED_VERSIONING("UPDATE in WRITE CONCURRENT");
  return maria_update(file, old_data, new_data);
}