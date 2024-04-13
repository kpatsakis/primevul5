MagickExport MagickBooleanType SolarizeImageChannel(Image *image,
  const ChannelType channel,const double threshold,ExceptionInfo *exception)
{
#define SolarizeImageTag  "Solarize/Image"

  CacheView
    *image_view;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (IsGrayColorspace(image->colorspace) != MagickFalse)
    (void) SetImageColorspace(image,sRGBColorspace);
  if (image->storage_class == PseudoClass)
    {
      ssize_t
        i;

      /*
        Solarize colormap.
      */
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        if ((channel & RedChannel) != 0)
          if ((double) image->colormap[i].red > threshold)
            image->colormap[i].red=QuantumRange-image->colormap[i].red;
        if ((channel & GreenChannel) != 0)
          if ((double) image->colormap[i].green > threshold)
            image->colormap[i].green=QuantumRange-image->colormap[i].green;
        if ((channel & BlueChannel) != 0)
          if ((double) image->colormap[i].blue > threshold)
            image->colormap[i].blue=QuantumRange-image->colormap[i].blue;
      }
    }
  /*
    Solarize image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    ssize_t
      x;

    PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,
      exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      if ((channel & RedChannel) != 0)
        if ((double) GetPixelRed(q) > threshold)
          SetPixelRed(q,QuantumRange-GetPixelRed(q));
      if ((channel & GreenChannel) != 0)
        if ((double) GetPixelGreen(q) > threshold)
          SetPixelGreen(q,QuantumRange-GetPixelGreen(q));
      if ((channel & BlueChannel) != 0)
        if ((double) GetPixelBlue(q) > threshold)
          SetPixelBlue(q,QuantumRange-GetPixelBlue(q));
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,SolarizeImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}