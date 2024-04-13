append_string (const char *str, struct growable *dest)
{
  int l = strlen (str);

  if (l)
    {
      GROW (dest, l);
      memcpy (TAIL (dest), str, l);
      TAIL_INCR (dest, l);
    }

  append_null (dest);
}