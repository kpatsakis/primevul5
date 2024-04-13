static inline MagickBooleanType HEICSkipImage(const ImageInfo *image_info,
  Image *image)
{
  if (image_info->number_scenes == 0)
    return(MagickFalse);
  if (image->scene == 0)
    return(MagickFalse);
  if (image->scene < image_info->scene)
    return(MagickTrue);
  if (image->scene > image_info->scene+image_info->number_scenes-1)
    return(MagickTrue);
  return(MagickFalse);
}