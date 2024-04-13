MagickExport Image *SwirlImage(const Image *image,double degrees,
  ExceptionInfo *exception)
{
#define SwirlImageTag  "Swirl/Image"

  CacheView
    *image_view,
    *swirl_view;

  double
    radius;

  Image
    *swirl_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    zero;

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
  swirl_image=CloneImage(image,0,0,MagickTrue,exception);
  if (swirl_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(swirl_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&swirl_image->exception);
      swirl_image=DestroyImage(swirl_image);
      return((Image *) NULL);
    }
  if (swirl_image->background_color.opacity != OpaqueOpacity)
    swirl_image->matte=MagickTrue;
  /*
    Compute scaling factor.
  */
  center.x=(double) image->columns/2.0;
  center.y=(double) image->rows/2.0;
  radius=MagickMax(center.x,center.y);
  scale.x=1.0;
  scale.y=1.0;
  if (image->columns > image->rows)
    scale.y=(double) image->columns/(double) image->rows;
  else
    if (image->columns < image->rows)
      scale.x=(double) image->rows/(double) image->columns;
  degrees=(double) DegreesToRadians(degrees);
  /*
    Swirl image.
  */
  status=MagickTrue;
  progress=0;
  GetMagickPixelPacket(swirl_image,&zero);
  image_view=AcquireVirtualCacheView(image,exception);
  swirl_view=AcquireAuthenticCacheView(swirl_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,swirl_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    double
      distance;

    MagickPixelPacket
      pixel;

    PointInfo
      delta;

    IndexPacket
      *magick_restrict swirl_indexes;

    ssize_t
      x;

    PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(swirl_view,0,y,swirl_image->columns,1,
      exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    swirl_indexes=GetCacheViewAuthenticIndexQueue(swirl_view);
    delta.y=scale.y*(double) (y-center.y);
    pixel=zero;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      /*
        Determine if the pixel is within an ellipse.
      */
      delta.x=scale.x*(double) (x-center.x);
      distance=delta.x*delta.x+delta.y*delta.y;
      if (distance < (radius*radius))
        {
          double
            cosine,
            factor,
            sine;

          /*
            Swirl the pixel.
          */
          factor=1.0-sqrt(distance)/radius;
          sine=sin((double) (degrees*factor*factor));
          cosine=cos((double) (degrees*factor*factor));
          status=InterpolateMagickPixelPacket(image,image_view,
            UndefinedInterpolatePixel,(double) ((cosine*delta.x-sine*delta.y)/
            scale.x+center.x),(double) ((sine*delta.x+cosine*delta.y)/scale.y+
            center.y),&pixel,exception);
          if (status == MagickFalse)
            break;
          SetPixelPacket(swirl_image,&pixel,q,swirl_indexes+x);
        }
      q++;
    }
    if (SyncCacheViewAuthenticPixels(swirl_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,SwirlImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  swirl_view=DestroyCacheView(swirl_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    swirl_image=DestroyImage(swirl_image);
  return(swirl_image);
}