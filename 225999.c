MagickExport Image *PolynomialImage(const Image *images,
  const size_t number_terms,const double *terms,ExceptionInfo *exception)
{
#define PolynomialImageTag  "Polynomial/Image"

  CacheView
    *polynomial_view;

  Image
    *image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PixelChannels
    **magick_restrict polynomial_pixels;

  size_t
    number_images;

  ssize_t
    y;

  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImageCanvas(images,exception);
  if (image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
    {
      image=DestroyImage(image);
      return((Image *) NULL);
    }
  number_images=GetImageListLength(images);
  polynomial_pixels=AcquirePixelThreadSet(images);
  if (polynomial_pixels == (PixelChannels **) NULL)
    {
      image=DestroyImage(image);
      (void) ThrowMagickException(exception,GetMagickModule(),
        ResourceLimitError,"MemoryAllocationFailed","`%s'",images->filename);
      return((Image *) NULL);
    }
  /*
    Polynomial image pixels.
  */
  status=MagickTrue;
  progress=0;
  polynomial_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    CacheView
      *image_view;

    const Image
      *next;

    const int
      id = GetOpenMPThreadId();

    ssize_t
      i,
      x;

    PixelChannels
      *polynomial_pixel;

    Quantum
      *magick_restrict q;

    ssize_t
      j;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(polynomial_view,0,y,image->columns,1,
      exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    polynomial_pixel=polynomial_pixels[id];
    for (j=0; j < (ssize_t) image->columns; j++)
      for (i=0; i < MaxPixelChannels; i++)
        polynomial_pixel[j].channel[i]=0.0;
    next=images;
    for (j=0; j < (ssize_t) number_images; j++)
    {
      const Quantum
        *p;

      if (j >= (ssize_t) number_terms)
        continue;
      image_view=AcquireVirtualCacheView(next,exception);
      p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        {
          image_view=DestroyCacheView(image_view);
          break;
        }
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        ssize_t
          i;

        for (i=0; i < (ssize_t) GetPixelChannels(next); i++)
        {
          MagickRealType
            coefficient,
            degree;

          PixelChannel channel = GetPixelChannelChannel(image,i);
          PixelTrait traits = GetPixelChannelTraits(next,channel);
          PixelTrait polynomial_traits=GetPixelChannelTraits(image,channel);
          if ((traits == UndefinedPixelTrait) ||
              (polynomial_traits == UndefinedPixelTrait))
            continue;
          if ((traits & UpdatePixelTrait) == 0)
            continue;
          coefficient=(MagickRealType) terms[2*j];
          degree=(MagickRealType) terms[(j << 1)+1];
          polynomial_pixel[x].channel[i]+=coefficient*
            pow(QuantumScale*GetPixelChannel(image,channel,p),degree);
        }
        p+=GetPixelChannels(next);
      }
      image_view=DestroyCacheView(image_view);
      next=GetNextImageInList(next);
    }
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
        q[i]=ClampToQuantum(QuantumRange*polynomial_pixel[x].channel[i]);
      }
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(polynomial_view,exception) == MagickFalse)
      status=MagickFalse;
    if (images->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(images,PolynomialImageTag,progress,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  polynomial_view=DestroyCacheView(polynomial_view);
  polynomial_pixels=DestroyPixelThreadSet(images,polynomial_pixels);
  if (status == MagickFalse)
    image=DestroyImage(image);
  return(image);
}