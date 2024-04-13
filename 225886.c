MagickExport Image *AddNoiseImage(const Image *image,const NoiseType noise_type,
  ExceptionInfo *exception)
{
  Image
    *noise_image;

  noise_image=AddNoiseImageChannel(image,DefaultChannels,noise_type,exception);
  return(noise_image);
}