MagickExport Image *AdaptiveThresholdImage(const Image *image,
  const size_t width,const size_t height,const double bias,
  ExceptionInfo *exception)
{
#define AdaptiveThresholdImageTag  "AdaptiveThreshold/Image"

  CacheView
    *image_view,
    *threshold_view;

  Image
    *threshold_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickSizeType
    number_pixels;

  ssize_t
    y;

  /*
    Initialize threshold image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
   threshold_image=CloneImage(image,0,0,MagickTrue,exception);
   if (threshold_image == (Image *) NULL)
     return((Image *) NULL);
   status=SetImageStorageClass(threshold_image,DirectClass,exception);
   if (status == MagickFalse)
     {
      threshold_image=DestroyImage(threshold_image);
      return((Image *) NULL);
    }
  /*
    Threshold image.
  */
  status=MagickTrue;
  progress=0;
  number_pixels=(MagickSizeType) width*height;
  image_view=AcquireVirtualCacheView(image,exception);
  threshold_view=AcquireAuthenticCacheView(threshold_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,threshold_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    double
      channel_bias[MaxPixelChannels],
      channel_sum[MaxPixelChannels];

    register const Quantum
      *magick_restrict p,
      *magick_restrict pixels;

    register Quantum
      *magick_restrict q;

    register ssize_t
      i,
      x;

    ssize_t
      center,
      u,
      v;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,-((ssize_t) width/2L),y-(ssize_t)
      (height/2L),image->columns+width,height,exception);
    q=QueueCacheViewAuthenticPixels(threshold_view,0,y,threshold_image->columns,
      1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    center=(ssize_t) GetPixelChannels(image)*(image->columns+width)*(height/2L)+
      GetPixelChannels(image)*(width/2);
    for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
    {
      PixelChannel channel = GetPixelChannelChannel(image,i);
      PixelTrait traits = GetPixelChannelTraits(image,channel);
      PixelTrait threshold_traits=GetPixelChannelTraits(threshold_image,
        channel);
      if ((traits == UndefinedPixelTrait) ||
          (threshold_traits == UndefinedPixelTrait))
        continue;
      if ((threshold_traits & CopyPixelTrait) != 0)
        {
          SetPixelChannel(threshold_image,channel,p[center+i],q);
          continue;
        }
      pixels=p;
      channel_bias[channel]=0.0;
      channel_sum[channel]=0.0;
      for (v=0; v < (ssize_t) height; v++)
      {
        for (u=0; u < (ssize_t) width; u++)
        {
          if (u == (ssize_t) (width-1))
            channel_bias[channel]+=pixels[i];
          channel_sum[channel]+=pixels[i];
          pixels+=GetPixelChannels(image);
        }
        pixels+=GetPixelChannels(image)*image->columns;
      }
    }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        double
          mean;

        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        PixelTrait threshold_traits=GetPixelChannelTraits(threshold_image,
          channel);
        if ((traits == UndefinedPixelTrait) ||
            (threshold_traits == UndefinedPixelTrait))
          continue;
        if ((threshold_traits & CopyPixelTrait) != 0)
          {
            SetPixelChannel(threshold_image,channel,p[center+i],q);
            continue;
          }
        channel_sum[channel]-=channel_bias[channel];
        channel_bias[channel]=0.0;
        pixels=p;
        for (v=0; v < (ssize_t) height; v++)
        {
          channel_bias[channel]+=pixels[i];
          pixels+=(width-1)*GetPixelChannels(image);
          channel_sum[channel]+=pixels[i];
          pixels+=GetPixelChannels(image)*(image->columns+1);
        }
        mean=(double) (channel_sum[channel]/number_pixels+bias);
        SetPixelChannel(threshold_image,channel,(Quantum) ((double)
          p[center+i] <= mean ? 0 : QuantumRange),q);
      }
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(threshold_image);
    }
    if (SyncCacheViewAuthenticPixels(threshold_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,AdaptiveThresholdImageTag,progress,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  threshold_image->type=image->type;
  threshold_view=DestroyCacheView(threshold_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    threshold_image=DestroyImage(threshold_image);
  return(threshold_image);
}
