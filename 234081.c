ds_append (dynamic_string *s, int c)
{
  ds_resize (s);
  s->ds_string[s->ds_idx] = c;
  if (c)
    {
      s->ds_idx++;
      ds_resize (s);
      s->ds_string[s->ds_idx] = 0;
    }      
}