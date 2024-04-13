rle_write (FILE   *fp,
           guchar *buffer,
           guint   width,
           guint   bytes)
{
  gint    repeat = 0;
  gint    direct = 0;
  guchar *from   = buffer;
  guint   x;

  for (x = 1; x < width; ++x)
    {
      if (memcmp (buffer, buffer + bytes, bytes))
        {
          /* next pixel is different */
          if (repeat)
            {
              putc (128 + repeat, fp);
              fwrite (from, bytes, 1, fp);
              from = buffer + bytes; /* point to first different pixel */
              repeat = 0;
              direct = 0;
            }
          else
            {
              direct += 1;
            }
        }
      else
        {
          /* next pixel is the same */
          if (direct)
            {
              putc (direct - 1, fp);
              fwrite (from, bytes, direct, fp);
              from = buffer; /* point to first identical pixel */
              direct = 0;
              repeat = 1;
            }
          else
            {
              repeat += 1;
            }
        }

      if (repeat == 128)
        {
          putc (255, fp);
          fwrite (from, bytes, 1, fp);
          from = buffer + bytes;
          direct = 0;
          repeat = 0;
        }
      else if (direct == 128)
        {
          putc (127, fp);
          fwrite (from, bytes, direct, fp);
          from = buffer+ bytes;
          direct = 0;
          repeat = 0;
        }

      buffer += bytes;
    }

  if (repeat > 0)
    {
      putc (128 + repeat, fp);
      fwrite (from, bytes, 1, fp);
    }
  else
    {
      putc (direct, fp);
      fwrite (from, bytes, direct + 1, fp);
    }
}