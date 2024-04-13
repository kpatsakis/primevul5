upsample (guchar       *dest,
          const guchar *src,
          guint         width,
          guint         bytes,
          guint         alpha)
{
  guint x;

  for (x = 0; x < width; x++)
    {
      dest[0] =  ((src[1] << 1) & 0xf8);
      dest[0] += (dest[0] >> 5);

      dest[1] =  ((src[0] & 0xe0) >> 2) + ((src[1] & 0x03) << 6);
      dest[1] += (dest[1] >> 5);

      dest[2] =  ((src[0] << 3) & 0xf8);
      dest[2] += (dest[2] >> 5);

      if (alpha)
        {
          dest[3] = (src[1] & 0x80) ? 255 : 0;
          dest += 4;
        }
      else
        {
          dest += 3;
        }

      src += bytes;
    }
}