static SVGInfo *DestroySVGInfo(SVGInfo *svg_info)
{
  if (svg_info->size != (char *) NULL)
    svg_info->size=DestroyString(svg_info->size);
  if (svg_info->text != (char *) NULL)
    svg_info->text=DestroyString(svg_info->text);
  if (svg_info->scale != (double *) NULL)
    svg_info->scale=(double *) RelinquishMagickMemory(svg_info->scale);
  if (svg_info->title != (char *) NULL)
    svg_info->title=DestroyString(svg_info->title);
  if (svg_info->comment != (char *) NULL)
    svg_info->comment=DestroyString(svg_info->comment);
  return((SVGInfo *) RelinquishMagickMemory(svg_info));
}