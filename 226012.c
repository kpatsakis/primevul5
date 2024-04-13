MagickExport MagickBooleanType GetImageRange(const Image *image,double *minima,
  double *maxima,ExceptionInfo *exception)
{
  CacheView
    *image_view;

  MagickBooleanType
    initialize,
    status;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=MagickTrue;
  initialize=MagickTrue;
  *maxima=0.0;
  *minima=0.0;
  image_view=AcquireVirtualCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status,initialize) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    double
      row_maxima = 0.0,
      row_minima = 0.0;

    MagickBooleanType
      row_initialize;

    const Quantum
      *magick_restrict p;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    row_initialize=MagickTrue;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if (traits == UndefinedPixelTrait)
          continue;
        if ((traits & UpdatePixelTrait) == 0)
          continue;
				if (row_initialize != MagickFalse)
          {
            row_minima=(double) p[i];
            row_maxima=(double) p[i];
            row_initialize=MagickFalse;
          }
        else
          {
            if ((double) p[i] < row_minima)
              row_minima=(double) p[i];
            if ((double) p[i] > row_maxima)
              row_maxima=(double) p[i];
         }
      }
      p+=GetPixelChannels(image);
    }
#if defined(MAGICKCORE_OPENMP_SUPPORT)
#pragma omp critical (MagickCore_GetImageRange)
#endif
    {
      if (initialize != MagickFalse)
        {
          *minima=row_minima;
          *maxima=row_maxima;
          initialize=MagickFalse;
        }
      else
        {
          if (row_minima < *minima)
            *minima=row_minima;
          if (row_maxima > *maxima)
            *maxima=row_maxima;
        }
    }
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}