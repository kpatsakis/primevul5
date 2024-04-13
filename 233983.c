append_char (char ch, struct growable *dest)
{
  if (ch)
    {
      GROW (dest, 1);
      *TAIL (dest) = ch;
      TAIL_INCR (dest, 1);
    }

  append_null (dest);
}