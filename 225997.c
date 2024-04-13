MagickExport Image *StatisticImage(const Image *image,const StatisticType type,
  const size_t width,const size_t height,ExceptionInfo *exception)
{
#define StatisticImageTag  "Statistic/Image"

  CacheView
    *image_view,
    *statistic_view;

  Image
    *statistic_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PixelList
    **magick_restrict pixel_list;

  ssize_t
    center,
    y;

  /*
    Initialize statistics image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  statistic_image=CloneImage(image,0,0,MagickTrue,
    exception);
  if (statistic_image == (Image *) NULL)
    return((Image *) NULL);
  status=SetImageStorageClass(statistic_image,DirectClass,exception);
  if (status == MagickFalse)
    {
      statistic_image=DestroyImage(statistic_image);
      return((Image *) NULL);
    }
  pixel_list=AcquirePixelListThreadSet(MagickMax(width,1),MagickMax(height,1));
  if (pixel_list == (PixelList **) NULL)
    {
      statistic_image=DestroyImage(statistic_image);
      ThrowImageException(ResourceLimitError,"MemoryAllocationFailed");
    }
  /*
    Make each pixel the min / max / median / mode / etc. of the neighborhood.
  */
  center=(ssize_t) GetPixelChannels(image)*(image->columns+MagickMax(width,1))*
    (MagickMax(height,1)/2L)+GetPixelChannels(image)*(MagickMax(width,1)/2L);
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  statistic_view=AcquireAuthenticCacheView(statistic_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,statistic_image,statistic_image->rows,1)
#endif
  for (y=0; y < (ssize_t) statistic_image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    const Quantum
      *magick_restrict p;

    Quantum
      *magick_restrict q;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,-((ssize_t) MagickMax(width,1)/2L),y-
      (ssize_t) (MagickMax(height,1)/2L),image->columns+MagickMax(width,1),
      MagickMax(height,1),exception);
    q=QueueCacheViewAuthenticPixels(statistic_view,0,y,statistic_image->columns,      1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) statistic_image->columns; x++)
    {
      ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        double
          area,
          maximum,
          minimum,
          sum,
          sum_squared;

        Quantum
          pixel;

        const Quantum
          *magick_restrict pixels;

        ssize_t
          u;

        ssize_t
          v;

        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        PixelTrait statistic_traits=GetPixelChannelTraits(statistic_image,
          channel);
        if ((traits == UndefinedPixelTrait) ||
            (statistic_traits == UndefinedPixelTrait))
          continue;
        if (((statistic_traits & CopyPixelTrait) != 0) ||
            (GetPixelWriteMask(image,p) <= (QuantumRange/2)))
          {
            SetPixelChannel(statistic_image,channel,p[center+i],q);
            continue;
          }
        if ((statistic_traits & UpdatePixelTrait) == 0)
          continue;
        pixels=p;
        area=0.0;
        minimum=pixels[i];
        maximum=pixels[i];
        sum=0.0;
        sum_squared=0.0;
        ResetPixelList(pixel_list[id]);
        for (v=0; v < (ssize_t) MagickMax(height,1); v++)
        {
          for (u=0; u < (ssize_t) MagickMax(width,1); u++)
          {
            if ((type == MedianStatistic) || (type == ModeStatistic) ||
                (type == NonpeakStatistic))
              {
                InsertPixelList(pixels[i],pixel_list[id]);
                pixels+=GetPixelChannels(image);
                continue;
              }
            area++;
            if (pixels[i] < minimum)
              minimum=(double) pixels[i];
            if (pixels[i] > maximum)
              maximum=(double) pixels[i];
            sum+=(double) pixels[i];
            sum_squared+=(double) pixels[i]*pixels[i];
            pixels+=GetPixelChannels(image);
          }
          pixels+=GetPixelChannels(image)*image->columns;
        }
        switch (type)
        {
          case GradientStatistic:
          {
            pixel=ClampToQuantum(MagickAbsoluteValue(maximum-minimum));
            break;
          }
          case MaximumStatistic:
          {
            pixel=ClampToQuantum(maximum);
            break;
          }
          case MeanStatistic:
          default:
          {
            pixel=ClampToQuantum(sum/area);
            break;
          }
          case MedianStatistic:
          {
            GetMedianPixelList(pixel_list[id],&pixel);
            break;
          }
          case MinimumStatistic:
          {
            pixel=ClampToQuantum(minimum);
            break;
          }
          case ModeStatistic:
          {
            GetModePixelList(pixel_list[id],&pixel);
            break;
          }
          case NonpeakStatistic:
          {
            GetNonpeakPixelList(pixel_list[id],&pixel);
            break;
          }
          case RootMeanSquareStatistic:
          {
            pixel=ClampToQuantum(sqrt(sum_squared/area));
            break;
          }
          case StandardDeviationStatistic:
          {
            pixel=ClampToQuantum(sqrt(sum_squared/area-(sum/area*sum/area)));
            break;
          }
        }
        SetPixelChannel(statistic_image,channel,pixel,q);
      }
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(statistic_image);
    }
    if (SyncCacheViewAuthenticPixels(statistic_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,StatisticImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  statistic_view=DestroyCacheView(statistic_view);
  image_view=DestroyCacheView(image_view);
  pixel_list=DestroyPixelListThreadSet(pixel_list);
  if (status == MagickFalse)
    statistic_image=DestroyImage(statistic_image);
  return(statistic_image);
}