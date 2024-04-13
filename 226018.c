MagickExport ChannelPerceptualHash *GetImagePerceptualHash(const Image *image,
  ExceptionInfo *exception)
{
  ChannelPerceptualHash
    *perceptual_hash;

  char
    *colorspaces,
    *p,
    *q;

  const char
    *artifact;

  MagickBooleanType
    status;

  ssize_t
    i;

  perceptual_hash=(ChannelPerceptualHash *) AcquireQuantumMemory(
    MaxPixelChannels+1UL,sizeof(*perceptual_hash));
  if (perceptual_hash == (ChannelPerceptualHash *) NULL)
    return((ChannelPerceptualHash *) NULL);
  artifact=GetImageArtifact(image,"phash:colorspaces");
  if (artifact != NULL)
    colorspaces=AcquireString(artifact);
  else
    colorspaces=AcquireString("sRGB,HCLp");
  perceptual_hash[0].number_colorspaces=0;
  perceptual_hash[0].number_channels=0;
  q=colorspaces;
  for (i=0; (p=StringToken(",",&q)) != (char *) NULL; i++)
  {
    ChannelMoments
      *moments;

    Image
      *hash_image;

    size_t
      j;

    ssize_t
      channel,
      colorspace;

    if (i >= MaximumNumberOfPerceptualColorspaces)
      break;
    colorspace=ParseCommandOption(MagickColorspaceOptions,MagickFalse,p);
    if (colorspace < 0)
      break;
    perceptual_hash[0].colorspace[i]=(ColorspaceType) colorspace;
    hash_image=BlurImage(image,0.0,1.0,exception);
    if (hash_image == (Image *) NULL)
      break;
    hash_image->depth=8;
    status=TransformImageColorspace(hash_image,(ColorspaceType) colorspace,
      exception);
    if (status == MagickFalse)
      break;
    moments=GetImageMoments(hash_image,exception);
    perceptual_hash[0].number_colorspaces++;
    perceptual_hash[0].number_channels+=GetImageChannels(hash_image);
    hash_image=DestroyImage(hash_image);
    if (moments == (ChannelMoments *) NULL)
      break;
    for (channel=0; channel <= MaxPixelChannels; channel++)
      for (j=0; j < MaximumNumberOfImageMoments; j++)
        perceptual_hash[channel].phash[i][j]=
          (-MagickLog10(moments[channel].invariant[j]));
    moments=(ChannelMoments *) RelinquishMagickMemory(moments);
  }
  colorspaces=DestroyString(colorspaces);
  return(perceptual_hash);
}