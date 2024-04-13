static size_t GetImageChannels(const Image *image)
{
  ssize_t
    i;

  size_t
    channels;

  channels=0;
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    PixelChannel channel = GetPixelChannelChannel(image,i);
    PixelTrait traits = GetPixelChannelTraits(image,channel);
    if (traits == UndefinedPixelTrait)
      continue;
    if ((traits & UpdatePixelTrait) == 0)
      continue;
    channels++;
  }
  return((size_t) (channels == 0 ? 1 : channels));
}