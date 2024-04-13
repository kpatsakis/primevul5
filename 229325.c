uchar *ma_get_hash_keyval(const uchar *hash_entry,
                       unsigned int *length,
                       my_bool not_used __attribute__((unused)))
{
  /* Hash entry has the following format:
     Offset: 0               key (\0 terminated)
             key_length + 1  value (\0 terminated)
  */
  uchar *p= (uchar *)hash_entry;
  size_t len= strlen((char *)p);
  *length= (unsigned int)len;
  return p;
}