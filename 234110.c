ds_endswith (dynamic_string *s, int c)
{
  return (s->ds_idx > 0 && s->ds_string[s->ds_idx - 1] == c);
}