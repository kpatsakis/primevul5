read_line (FILE         *fp,
           guchar       *row,
           guchar       *buffer,
           tga_info     *info,
           GimpDrawable *drawable,
           const guchar *convert_cmap)
{
  if (info->imageCompression == TGA_COMP_RLE)
    {
      rle_read (fp, buffer, info);
    }
  else
    {
      fread (buffer, info->bytes, info->width, fp);
    }

  if (info->flipHoriz)
    {
      flip_line (buffer, info);
    }

  if (info->imageType == TGA_TYPE_COLOR)
    {
      if (info->bpp == 16 || info->bpp == 15)
        {
          upsample (row, buffer, info->width, info->bytes, info->alphaBits);
        }
      else
        {
          bgr2rgb (row, buffer, info->width, info->bytes, info->alphaBits);
        }
    }
  else if (convert_cmap)
    {
      gboolean has_alpha = (info->alphaBits > 0);

      apply_colormap (row, buffer, info->width, convert_cmap, has_alpha,
                      info->colorMapIndex);
    }
  else if (info->imageType == TGA_TYPE_MAPPED)
    {
      g_assert(drawable->bpp == 1);

      apply_index (row, buffer, info->width, info->colorMapIndex);
    }
  else
    {
      memcpy (row, buffer, info->width * drawable->bpp);
    }
}