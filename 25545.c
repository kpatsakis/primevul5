uchar *get_bookmark_hash_key(const uchar *buff, size_t *length,
                             my_bool not_used __attribute__((unused)))
{
  struct st_bookmark *var= (st_bookmark *)buff;
  *length= var->name_len + 1;
  return (uchar*) var->key;
}