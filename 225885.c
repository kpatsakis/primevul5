MagickExport MagickBooleanType SolarizeImage(Image *image,
  const double threshold)
{
  MagickBooleanType
    status;

  status=SolarizeImageChannel(image,DefaultChannels,threshold,
    &image->exception);
  return(status);
}