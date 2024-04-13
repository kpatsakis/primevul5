FT_INFO *ha_maria::ft_init_ext(uint flags, uint inx, String * key)
{
  return maria_ft_init_search(flags, file, inx,
                              (uchar *) key->ptr(), key->length(),
                              key->charset(), table->record[0]);
}