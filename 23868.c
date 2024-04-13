flip_line (guchar   *buf,
           tga_info *info)
{
  guchar  temp;
  guchar *alt;
  gint    x, s;

  alt = buf + (info->bytes * (info->width - 1));

  for (x = 0; x * 2 < info->width; x++)
    {
      for (s = 0; s < info->bytes; ++s)
        {
          temp = buf[s];
          buf[s] = alt[s];
          alt[s] = temp;
        }

      buf += info->bytes;
      alt -= info->bytes;
    }
}