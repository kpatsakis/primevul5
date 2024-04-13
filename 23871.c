apply_colormap (guchar       *dest,
                const guchar *src,
                guint         width,
                const guchar *cmap,
                gboolean      alpha,
                guint16       index)
{
  guint x;

  if (alpha)
    {
      for (x = 0; x < width; x++)
        {
          *(dest++) = cmap[(*src - index) * 4];
          *(dest++) = cmap[(*src - index) * 4 + 1];
          *(dest++) = cmap[(*src - index) * 4 + 2];
          *(dest++) = cmap[(*src - index) * 4 + 3];

          src++;
        }
    }
  else
    {
      for (x = 0; x < width; x++)
        {
          *(dest++) = cmap[(*src - index) * 3];
          *(dest++) = cmap[(*src - index) * 3 + 1];
          *(dest++) = cmap[(*src - index) * 3 + 2];

          src++;
        }
    }
}