MagickExport Image *SwirlImage(const Image *image,double degrees,
  const PixelInterpolateMethod method,ExceptionInfo *exception)
{
#define SwirlImageTag  "Swirl/Image"

  CacheView
    *canvas_view,
    *interpolate_view,
    *swirl_view;

  double
    radius;

  Image
    *canvas_image,
    *swirl_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PointInfo
    center,
    scale;

  ssize_t
    y;

  /*
    Initialize swirl image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  canvas_image=CloneImage(image,0,0,MagickTrue,exception);
  if (canvas_image == (Image *) NULL)
    return((Image *) NULL);
  swirl_image=CloneImage(canvas_image,0,0,MagickTrue,exception);
  if (swirl_image == (Image *) NULL)
    {
      canvas_image=DestroyImage(canvas_image);
      return((Image *) NULL);
    }
  if (SetImageStorageClass(swirl_image,DirectClass,exception) == MagickFalse)
    {
      canvas_image=DestroyImage(canvas_image);
      swirl_image=DestroyImage(swirl_image);
      return((Image *) NULL);
    }
  if (swirl_image->background_color.alpha_trait != UndefinedPixelTrait)
    (void) SetImageAlphaChannel(swirl_image,OnAlphaChannel,exception);
  /*
    Compute scaling factor.
  */
  center.x=(double) canvas_image->columns/2.0;
  center.y=(double) canvas_image->rows/2.0;
  radius=MagickMax(center.x,center.y);
  scale.x=1.0;
  scale.y=1.0;
  if (canvas_image->columns > canvas_image->rows)
    scale.y=(double) canvas_image->columns/(double) canvas_image->rows;
  else
    if (canvas_image->columns < canvas_image->rows)
      scale.x=(double) canvas_image->rows/(double) canvas_image->columns;
  degrees=(double) DegreesToRadians(degrees);
  /*
    Swirl image.
  */
  status=MagickTrue;
  progress=0;
  canvas_view=AcquireVirtualCacheView(canvas_image,exception);
  interpolate_view=AcquireVirtualCacheView(image,exception);
  swirl_view=AcquireAuthenticCacheView(swirl_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(canvas_image,swirl_image,canvas_image->rows,1)
#endif
  for (y=0; y < (ssize_t) canvas_image->rows; y++)
  {
    double
      distance;

    PointInfo
      delta;

    const Quantum
      *magick_restrict p;

    ssize_t
      x;

    Quantum
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(canvas_view,0,y,canvas_image->columns,1,
      exception);
    q=QueueCacheViewAuthenticPixels(swirl_view,0,y,swirl_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    delta.y=scale.y*(double) (y-center.y);
    for (x=0; x < (ssize_t) canvas_image->columns; x++)
    {
      /*
        Determine if the pixel is within an ellipse.
      */
      delta.x=scale.x*(double) (x-center.x);
      distance=delta.x*delta.x+delta.y*delta.y;
      if (distance >= (radius*radius))
        {
          ssize_t
            i;

          for (i=0; i < (ssize_t) GetPixelChannels(canvas_image); i++)
          {
            PixelChannel channel = GetPixelChannelChannel(canvas_image,i);
            PixelTrait traits = GetPixelChannelTraits(canvas_image,channel);
            PixelTrait swirl_traits = GetPixelChannelTraits(swirl_image,
              channel);
            if ((traits == UndefinedPixelTrait) ||
                (swirl_traits == UndefinedPixelTrait))
              continue;
            SetPixelChannel(swirl_image,channel,p[i],q);
          }
        }
      else
        {
          double
            cosine,
            factor,
            sine;

          /*
            Swirl the pixel.
          */
          factor=1.0-sqrt((double) distance)/radius;
          sine=sin((double) (degrees*factor*factor));
          cosine=cos((double) (degrees*factor*factor));
          status=InterpolatePixelChannels(canvas_image,interpolate_view,
            swirl_image,method,((cosine*delta.x-sine*delta.y)/scale.x+center.x),
            (double) ((sine*delta.x+cosine*delta.y)/scale.y+center.y),q,
            exception);
          if (status == MagickFalse)
            break;
        }
      p+=GetPixelChannels(canvas_image);
      q+=GetPixelChannels(swirl_image);
    }
    if (SyncCacheViewAuthenticPixels(swirl_view,exception) == MagickFalse)
      status=MagickFalse;
    if (canvas_image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(canvas_image,SwirlImageTag,progress,
          canvas_image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  swirl_view=DestroyCacheView(swirl_view);
  interpolate_view=DestroyCacheView(interpolate_view);
  canvas_view=DestroyCacheView(canvas_view);
  canvas_image=DestroyImage(canvas_image);
  if (status == MagickFalse)
    swirl_image=DestroyImage(swirl_image);
  return(swirl_image);
}