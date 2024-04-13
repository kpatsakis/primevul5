bgr2rgb (guchar       *dest,
         const guchar *src,
         guint         width,
         guint         bytes,
         guint         alpha)
{
  guint x;

  if (alpha)
    {
      for (x = 0; x < width; x++)
        {
          *(dest++) = src[2];
          *(dest++) = src[1];
          *(dest++) = src[0];
          *(dest++) = src[3];

          src += bytes;
        }
    }
  else
    {
      for (x = 0; x < width; x++)
        {
          *(dest++) = src[2];
          *(dest++) = src[1];
          *(dest++) = src[0];

          src += bytes;
        }
    }
}