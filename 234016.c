rle_read (FILE     *fp,
          guchar   *buffer,
          tga_info *info)
{
  static gint   repeat = 0;
  static gint   direct = 0;
  static guchar sample[4];
  gint head;
  gint x, k;

  for (x = 0; x < info->width; x++)
    {
      if (repeat == 0 && direct == 0)
        {
          head = getc (fp);

          if (head == EOF)
            {
              return EOF;
            }
          else if (head >= 128)
            {
              repeat = head - 127;

              if (fread (sample, info->bytes, 1, fp) < 1)
                return EOF;
            }
          else
            {
              direct = head + 1;
            }
        }

      if (repeat > 0)
        {
          for (k = 0; k < info->bytes; ++k)
            {
              buffer[k] = sample[k];
            }

          repeat--;
        }
      else /* direct > 0 */
        {
          if (fread (buffer, info->bytes, 1, fp) < 1)
            return EOF;

          direct--;
        }

      buffer += info->bytes;
    }

  return 0;
}