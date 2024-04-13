static int IntensityCompare(const void *x,const void *y)
{
  const PixelChannels
    *color_1,
    *color_2;

  double
    distance;

  ssize_t
    i;

  color_1=(const PixelChannels *) x;
  color_2=(const PixelChannels *) y;
  distance=0.0;
  for (i=0; i < MaxPixelChannels; i++)
    distance+=color_1->channel[i]-(double) color_2->channel[i];
  return(distance < 0.0 ? -1 : distance > 0.0 ? 1 : 0);
}