ds_reset (dynamic_string *s, size_t len)
{
  while (len > s->ds_size)
    ds_resize (s);
  s->ds_idx = len;
}