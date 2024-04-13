ReadImage (FILE        *fp,
           tga_info    *info,
           const gchar *filename)
{
  static gint32      image_ID;
  gint32             layer_ID;

  GimpPixelRgn       pixel_rgn;
  GimpDrawable      *drawable;
  guchar            *data, *buffer, *row;
  GimpImageType      dtype = 0;
  GimpImageBaseType  itype = 0;
  gint               i, y;

  gint               max_tileheight, tileheight;

  guint              cmap_bytes = 0;
  guchar            *tga_cmap = NULL;
  guchar            *gimp_cmap = NULL;
  guchar            *convert_cmap = NULL;

  switch (info->imageType)
    {
    case TGA_TYPE_MAPPED:
      cmap_bytes = (info->colorMapSize + 7 ) / 8;
      tga_cmap = g_new (guchar, info->colorMapLength * cmap_bytes);

      if (info->colorMapSize > 24)
        {
          /* indexed + full alpha => promoted to RGBA */
          itype = GIMP_RGB;
          dtype = GIMP_RGBA_IMAGE;
          convert_cmap = g_new (guchar, info->colorMapLength * 4);
        }
      else if (info->colorMapIndex + info->colorMapLength > 256)
        {
          /* more than 256 colormap entries => promoted to RGB */
          itype = GIMP_RGB;
          dtype = GIMP_RGB_IMAGE;
          convert_cmap = g_new (guchar, info->colorMapLength * 3);
        }
      else if (info->alphaBits > 0)
        {
          /* if alpha exists here, promote to RGB */
          itype = GIMP_RGB;
          dtype = GIMP_RGBA_IMAGE;
          convert_cmap = g_new (guchar, info->colorMapLength * 4);
        }
      else
        {
          itype = GIMP_INDEXED;
          dtype = GIMP_INDEXED_IMAGE;
          gimp_cmap = g_new (guchar, info->colorMapLength * 3);
        }
      break;

    case TGA_TYPE_GRAY:
      itype = GIMP_GRAY;

      if (info->alphaBits)
        dtype = GIMP_GRAYA_IMAGE;
      else
        dtype = GIMP_GRAY_IMAGE;
      break;

    case TGA_TYPE_COLOR:
      itype = GIMP_RGB;

      if (info->alphaBits)
        dtype = GIMP_RGBA_IMAGE;
      else
        dtype = GIMP_RGB_IMAGE;
      break;
    }

  /* Handle colormap */

  if (info->imageType == TGA_TYPE_MAPPED)
    {
      if (cmap_bytes <= 4 &&
          fread (tga_cmap, info->colorMapLength * cmap_bytes, 1, fp) == 1)
        {
          if (convert_cmap)
            {
              if (info->colorMapSize == 32)
                bgr2rgb (convert_cmap, tga_cmap,
                         info->colorMapLength, cmap_bytes, 1);
              else if (info->colorMapSize == 24)
                bgr2rgb (convert_cmap, tga_cmap,
                         info->colorMapLength, cmap_bytes, 0);
              else if (info->colorMapSize == 16 || info->colorMapSize == 15)
                upsample (convert_cmap, tga_cmap,
                          info->colorMapLength, cmap_bytes, info->alphaBits);
              else
                {
                  g_message ("Unsupported colormap depth: %u",
                             info->colorMapSize);
                  return -1;
                }
            }
          else
            {
              if (info->colorMapSize == 24)
                bgr2rgb (gimp_cmap, tga_cmap,
                         info->colorMapLength, cmap_bytes, 0);
              else if (info->colorMapSize == 16 || info->colorMapSize == 15)
                upsample (gimp_cmap, tga_cmap,
                          info->colorMapLength, cmap_bytes, info->alphaBits);
              else
                {
                  g_message ("Unsupported colormap depth: %u",
                             info->colorMapSize);
                  return -1;
                }
            }
        }
      else
        {
          g_message ("File '%s' is truncated or corrupted",
                     gimp_filename_to_utf8 (filename));
          return -1;
        }
    }

  image_ID = gimp_image_new (info->width, info->height, itype);
  gimp_image_set_filename (image_ID, filename);

  if (gimp_cmap)
    gimp_image_set_colormap (image_ID, gimp_cmap, info->colorMapLength);

  layer_ID = gimp_layer_new (image_ID,
                             _("Background"),
                             info->width, info->height,
                             dtype, 100,
                             GIMP_NORMAL_MODE);

  gimp_image_insert_layer (image_ID, layer_ID, -1, 0);

  drawable = gimp_drawable_get (layer_ID);

  /* Prepare the pixel region. */
  gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0,
                       info->width, info->height, TRUE, FALSE);

  /* Allocate the data. */
  max_tileheight = gimp_tile_height ();
  data = g_new (guchar, info->width * max_tileheight * drawable->bpp);
  buffer = g_new (guchar, info->width * info->bytes);

  if (info->flipVert)
    {
      for (i = 0; i < info->height; i += tileheight)
        {
          tileheight = i ? max_tileheight : (info->height % max_tileheight);
          if (tileheight == 0)
            tileheight = max_tileheight;

          for (y = 1; y <= tileheight; ++y)
            {
              row = data + (info->width * drawable->bpp * (tileheight - y));
              read_line (fp, row, buffer, info, drawable, convert_cmap);
            }

          gimp_progress_update ((gdouble) (i + tileheight) /
                                (gdouble) info->height);
          gimp_pixel_rgn_set_rect (&pixel_rgn, data, 0,
                                   info->height - i - tileheight,
                                   info->width, tileheight);
        }
    }
  else
    {
      for (i = 0; i < info->height; i += max_tileheight)
        {
          tileheight = MIN (max_tileheight, info->height - i);

          for (y = 0; y < tileheight; ++y)
            {
              row= data + (info->width * drawable->bpp * y);
              read_line (fp, row, buffer, info, drawable, convert_cmap);
            }

          gimp_progress_update ((gdouble) (i + tileheight) /
                                (gdouble) info->height);
          gimp_pixel_rgn_set_rect (&pixel_rgn, data, 0, i,
                                   info->width, tileheight);
        }
    }
  gimp_progress_update (1.0);

  g_free (data);
  g_free (buffer);

  g_free (convert_cmap);
  g_free (gimp_cmap);
  g_free (tga_cmap);

  gimp_drawable_flush (drawable);
  gimp_drawable_detach (drawable);

  return image_ID;
}  /*read_image*/