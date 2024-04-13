apply_index (guchar       *dest,
             const guchar *src,
             guint         width,
             guint16       index)
{
  guint x;

  for (x = 0; x < width; x++)
    {
      *(dest++) = *(src++) - index;
    }
}