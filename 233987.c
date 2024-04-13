append_null (struct growable *dest)
{
  GROW (dest, 1);
  *TAIL (dest) = 0;
}