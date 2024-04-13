process (GeglOperation       *operation,
         GeglBuffer          *output,
         const GeglRectangle *result,
         gint                 level)
{
  GeglChantO   *o = GEGL_CHANT_PROPERTIES (operation);
  FILE         *fp;
  pnm_struct    img;
  GeglRectangle rect = {0,0,0,0};
  gboolean      ret = FALSE;

  fp = (!strcmp (o->path, "-") ? stdin : fopen (o->path,"rb"));

  if (!fp)
    return FALSE;

  if (!ppm_load_read_header (fp, &img))
    goto out;

  /* Allocating Array Size */

  /* Should use g_try_malloc(), but this causes crashes elsewhere because the
   * error signalled by returning FALSE isn't properly acted upon. Therefore
   * g_malloc() is used here which aborts if the requested memory size can't be
   * allocated causing a controlled crash. */
  img.data = (guchar*) g_malloc (img.numsamples * img.bpc);

  /* No-op without g_try_malloc(), see above. */
  if (! img.data)
    {
      g_warning ("Couldn't allocate %" G_GSIZE_FORMAT " bytes, giving up.", ((gsize)img.numsamples * img.bpc));
      goto out;
    }

  rect.height = img.height;
  rect.width = img.width;

  switch (img.bpc)
    {
    case 1:
      gegl_buffer_get (output, &rect, 1.0, babl_format ("R'G'B' u8"), img.data,
                       GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);
      break;

    case 2:
      gegl_buffer_get (output, &rect, 1.0, babl_format ("R'G'B' u16"), img.data,
                       GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);
      break;

    default:
      g_warning ("%s: Programmer stupidity error", G_STRLOC);
    }

  ppm_load_read_image (fp, &img);

  switch (img.bpc)
    {
    case 1:
      gegl_buffer_set (output, &rect, 0, babl_format ("R'G'B' u8"), img.data,
                       GEGL_AUTO_ROWSTRIDE);
      break;

    case 2:
      gegl_buffer_set (output, &rect, 0, babl_format ("R'G'B' u16"), img.data,
                       GEGL_AUTO_ROWSTRIDE);
      break;

    default:
      g_warning ("%s: Programmer stupidity error", G_STRLOC);
    }

  g_free (img.data);

  ret = TRUE;

 out:
  if (stdin != fp)
    fclose (fp);

  return ret;
}