MagickExport Image *ImplodeImage(const Image *image,const double amount,
  ExceptionInfo *exception)
{
#define ImplodeImageTag  "Implode/Image"

  CacheView
    *image_view,
    *implode_view;

  double
    radius;

  Image
    *implode_image;

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
    Initialize implode image attributes.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  implode_image=CloneImage(image,0,0,MagickTrue,exception);
  if (implode_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(implode_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&implode_image->exception);
      implode_image=DestroyImage(implode_image);
      return((Image *) NULL);
    }
  if (implode_image->background_color.opacity != OpaqueOpacity)
    implode_image->matte=MagickTrue;
  /*
    Compute scaling factor.
  */
  scale.x=1.0;
  scale.y=1.0;
  center.x=0.5*image->columns;
  center.y=0.5*image->rows;
  radius=center.x;
  if (image->columns > image->rows)
    scale.y=(double) image->columns*PerceptibleReciprocal((double)
      image->rows);
  else
    if (image->columns < image->rows)
      {
        scale.x=(double) image->rows*PerceptibleReciprocal((double)
          image->columns);
        radius=center.y;
      }
  /*
    Implode image.
  */
  status=MagickTrue;
  progress=0;
  GetMagickPixelPacket(implode_image,&zero);
  image_view=AcquireVirtualCacheView(image,exception);
  implode_view=AcquireAuthenticCacheView(implode_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,implode_image,image->rows,1)
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
      *magick_restrict implode_indexes;

    ssize_t
      x;

    PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(implode_view,0,y,implode_image->columns,1,
      exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    implode_indexes=GetCacheViewAuthenticIndexQueue(implode_view);
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
            factor;

          /*
            Implode the pixel.
          */
          factor=1.0;
          if (distance > 0.0)
            factor=pow(sin((double) (MagickPI*sqrt((double) distance)/
              radius/2)),-amount);
          status=InterpolateMagickPixelPacket(image,image_view,
            UndefinedInterpolatePixel,(double) (factor*delta.x/scale.x+
            center.x),(double) (factor*delta.y/scale.y+center.y),&pixel,
            exception);
          if (status == MagickFalse)
            break;
          SetPixelPacket(implode_image,&pixel,q,implode_indexes+x);
        }
      q++;
    }
    if (SyncCacheViewAuthenticPixels(implode_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,ImplodeImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  implode_view=DestroyCacheView(implode_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    implode_image=DestroyImage(implode_image);
  return(implode_image);
}