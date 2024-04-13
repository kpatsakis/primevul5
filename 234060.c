ds_concat (dynamic_string *s, char const *str)
{
  size_t len = strlen (str);
  while (len + 1 > s->ds_size)
    ds_resize (s);
  memcpy (s->ds_string + s->ds_idx, str, len);
  s->ds_idx += len;
  s->ds_string[s->ds_idx] = 0;
}