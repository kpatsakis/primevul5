int ha_maria::delete_row(const uchar * buf)
{
  CHECK_UNTIL_WE_FULLY_IMPLEMENTED_VERSIONING("DELETE in WRITE CONCURRENT");
  return maria_delete(file, buf);
}