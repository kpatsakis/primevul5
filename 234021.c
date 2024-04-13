save_image (const gchar  *filename,
            gint32        image_ID,
            gint32        drawable_ID,
            GError      **error)
{
  GimpPixelRgn   pixel_rgn;
  GimpDrawable  *drawable;
  GimpImageType  dtype;
  gint           width;
  gint           height;

  FILE     *fp;
  gint      out_bpp = 0;
  gboolean  status  = TRUE;
  gint      i, row;

  guchar  header[18];
  guchar  footer[26];
  guchar *pixels;
  guchar *data;

  gint    num_colors;
  guchar *gimp_cmap = NULL;

  drawable = gimp_drawable_get (drawable_ID);
  dtype    = gimp_drawable_type (drawable_ID);

  width  = drawable->width;
  height = drawable->height;

  if ((fp = g_fopen (filename, "wb")) == NULL)
    {
      g_set_error (error, G_FILE_ERROR, g_file_error_from_errno (errno),
                   _("Could not open '%s' for writing: %s"),
                   gimp_filename_to_utf8 (filename), g_strerror (errno));
      return FALSE;
    }

  gimp_progress_init_printf (_("Saving '%s'"),
                             gimp_filename_to_utf8 (filename));

  header[0] = 0; /* No image identifier / description */

  if (dtype == GIMP_INDEXED_IMAGE)
    {
      gimp_cmap = gimp_image_get_colormap (image_ID, &num_colors);

      header[1] = 1; /* cmap type */
      header[2] = (tsvals.rle) ? 9 : 1;
      header[3] = header[4] = 0; /* no offset */
      header[5] = num_colors % 256;
      header[6] = num_colors / 256;
      header[7] = 24; /* cmap size / bits */
    }
  else if (dtype == GIMP_INDEXEDA_IMAGE)
    {
      gimp_cmap = gimp_image_get_colormap (image_ID, &num_colors);

      header[1] = 1; /* cmap type */
      header[2] = (tsvals.rle) ? 9 : 1;
      header[3] = header[4] = 0; /* no offset */
      header[5] = (num_colors + 1) % 256;
      header[6] = (num_colors + 1) / 256;
      header[7] = 32; /* cmap size / bits */
    }
  else
    {
      header[1]= 0;

      if (dtype == GIMP_RGB_IMAGE || dtype == GIMP_RGBA_IMAGE)
        {
          header[2]= (tsvals.rle) ? 10 : 2;
        }
      else
        {
          header[2]= (tsvals.rle) ? 11 : 3;
        }

      header[3] = header[4] = header[5] = header[6] = header[7] = 0;
    }

  header[8]  = header[9] = 0;                           /* xorigin */
  header[10] = tsvals.origin ? 0 : (height % 256);      /* yorigin */
  header[11] = tsvals.origin ? 0 : (height / 256);      /* yorigin */


  header[12] = width % 256;
  header[13] = width / 256;

  header[14] = height % 256;
  header[15] = height / 256;

  switch (dtype)
    {
    case GIMP_INDEXED_IMAGE:
    case GIMP_GRAY_IMAGE:
    case GIMP_INDEXEDA_IMAGE:
      out_bpp = 1;
      header[16] = 8; /* bpp */
      header[17] = tsvals.origin ? 0 : 0x20; /* alpha + orientation */
      break;

    case GIMP_GRAYA_IMAGE:
      out_bpp = 2;
      header[16] = 16; /* bpp */
      header[17] = tsvals.origin ? 8 : 0x28; /* alpha + orientation */
      break;

    case GIMP_RGB_IMAGE:
      out_bpp = 3;
      header[16] = 24; /* bpp */
      header[17] = tsvals.origin ? 0 : 0x20; /* alpha + orientation */
      break;

    case GIMP_RGBA_IMAGE:
      out_bpp = 4;
      header[16] = 32; /* bpp */
      header[17] = tsvals.origin ? 8 : 0x28; /* alpha + orientation */
      break;
    }

  /* write header to front of file */
  fwrite (header, sizeof (header), 1, fp);

  if (dtype == GIMP_INDEXED_IMAGE)
    {
      /* write out palette */
      for (i = 0; i < num_colors; ++i)
        {
          fputc (gimp_cmap[(i * 3) + 2], fp);
          fputc (gimp_cmap[(i * 3) + 1], fp);
          fputc (gimp_cmap[(i * 3) + 0], fp);
        }
    }
  else if (dtype == GIMP_INDEXEDA_IMAGE)
    {
      /* write out palette */
      for (i = 0; i < num_colors; ++i)
        {
          fputc (gimp_cmap[(i * 3) + 2], fp);
          fputc (gimp_cmap[(i * 3) + 1], fp);
          fputc (gimp_cmap[(i * 3) + 0], fp);
          fputc (255, fp);
        }

      fputc (0, fp);
      fputc (0, fp);
      fputc (0, fp);
      fputc (0, fp);
    }

  gimp_tile_cache_ntiles ((width / gimp_tile_width ()) + 1);

  gimp_pixel_rgn_init (&pixel_rgn, drawable, 0, 0, width, height, FALSE, FALSE);

  pixels = g_new (guchar, width * drawable->bpp);
  data   = g_new (guchar, width * out_bpp);

  for (row = 0; row < height; ++row)
    {
      if (tsvals.origin)
        {
          gimp_pixel_rgn_get_row (&pixel_rgn,
                                  pixels, 0, height - (row + 1), width);
        }
      else
        {
          gimp_pixel_rgn_get_row (&pixel_rgn,
                                  pixels, 0, row, width);
        }

      if (dtype == GIMP_RGB_IMAGE)
        {
          bgr2rgb (data, pixels, width, drawable->bpp, 0);
        }
      else if (dtype == GIMP_RGBA_IMAGE)
        {
          bgr2rgb (data, pixels, width, drawable->bpp, 1);
        }
      else if (dtype == GIMP_INDEXEDA_IMAGE)
        {
          for (i = 0; i < width; ++i)
            {
              if (pixels[i * 2 + 1] > 127)
                data[i] = pixels[i * 2];
              else
                data[i] = num_colors;
            }
        }
      else
        {
          memcpy (data, pixels, width * drawable->bpp);
        }

      if (tsvals.rle)
        {
          rle_write (fp, data, width, out_bpp);
        }
      else
        {
          fwrite (data, width * out_bpp, 1, fp);
        }

      if (row % 16 == 0)
        gimp_progress_update ((gdouble) row / (gdouble) height);
    }
  gimp_progress_update (1.0);

  gimp_drawable_detach (drawable);

  g_free (data);
  g_free (pixels);

  /* footer must be the last thing written to file */
  memset (footer, 0, 8); /* No extensions, no developer directory */
  memcpy (footer + 8, magic, sizeof (magic)); /* magic signature */
  fwrite (footer, sizeof (footer), 1, fp);

  fclose (fp);

  return status;
}