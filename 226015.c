static PixelChannels **DestroyPixelThreadSet(const Image *images,
  PixelChannels **pixels)
{
  ssize_t
    i;

  size_t
    rows;

  assert(pixels != (PixelChannels **) NULL);
  rows=MagickMax(GetImageListLength(images),(size_t)
    GetMagickResourceLimit(ThreadResource));
  for (i=0; i < (ssize_t) rows; i++)
    if (pixels[i] != (PixelChannels *) NULL)
      pixels[i]=(PixelChannels *) RelinquishMagickMemory(pixels[i]);
  pixels=(PixelChannels **) RelinquishMagickMemory(pixels);
  return(pixels);
}