flip_line (guchar   *buffer,
           tga_info *info)
{
  guchar  temp;
  guchar *alt;
  gint    x, s;

  alt = buffer + (info->bytes * (info->width - 1));

  for (x = 0; x * 2 < info->width; x++)
    {
      for (s = 0; s < info->bytes; ++s)
        {
          temp = buffer[s];
          buffer[s] = alt[s];
          alt[s] = temp;
        }

      buffer += info->bytes;
      alt -= info->bytes;
    }
}