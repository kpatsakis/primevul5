read_line (FILE         *fp,
           guchar       *row,
           guchar       *buf,
           tga_info     *info,
           gint          bpp,
           const guchar *convert_cmap)
{
  if (info->imageCompression == TGA_COMP_RLE)
    {
      rle_read (fp, buf, info);
    }
  else
    {
      fread (buf, info->bytes, info->width, fp);
    }

  if (info->flipHoriz)
    {
      flip_line (buf, info);
    }

  if (info->imageType == TGA_TYPE_COLOR)
    {
      if (info->bpp == 16 || info->bpp == 15)
        {
          upsample (row, buf, info->width, info->bytes, info->alphaBits);
        }
      else
        {
          bgr2rgb (row, buf, info->width, info->bytes, info->alphaBits);
        }
    }
  else if (convert_cmap)
    {
      gboolean has_alpha = (info->alphaBits > 0);

      apply_colormap (row, buf, info->width, convert_cmap, has_alpha,
                      info->colorMapIndex);
    }
  else if (info->imageType == TGA_TYPE_MAPPED)
    {
      g_assert(bpp == 1);

      apply_index (row, buf, info->width, info->colorMapIndex);
    }
  else
    {
      memcpy (row, buf, info->width * bpp);
    }
}