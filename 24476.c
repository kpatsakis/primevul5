void ha_maria::position(const uchar *record)
{
  my_off_t row_position= maria_position(file);
  my_store_ptr(ref, ref_length, row_position);
}