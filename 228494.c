get_bounding_box (GeglOperation *operation)
{
  GeglChantO   *o = GEGL_CHANT_PROPERTIES (operation);
  GeglRectangle result = {0,0,0,0};
  pnm_struct    img;
  FILE         *fp;

  fp = (!strcmp (o->path, "-") ? stdin : fopen (o->path,"rb") );

  if (!fp)
    return result;

  if (!ppm_load_read_header (fp, &img))
    goto out;

  switch (img.bpc)
    {
    case 1:
      gegl_operation_set_format (operation, "output",
                                 babl_format ("R'G'B' u8"));
      break;

    case 2:
      gegl_operation_set_format (operation, "output",
                                 babl_format ("R'G'B' u16"));
      break;

    default:
      g_warning ("%s: Programmer stupidity error", G_STRLOC);
    }

  result.width = img.width;
  result.height = img.height;

 out:
  if (stdin != fp)
    fclose (fp);

  return result;
}