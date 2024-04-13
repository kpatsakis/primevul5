MagickExport Image *BlueShiftImage(const Image *image,const double factor,
  ExceptionInfo *exception)
{
#define BlueShiftImageTag  "BlueShift/Image"

  CacheView
    *image_view,
    *shift_view;

  Image
    *shift_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  /*
    Allocate blue shift image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  shift_image=CloneImage(image,0,0,MagickTrue,exception);
  if (shift_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(shift_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&shift_image->exception);
      shift_image=DestroyImage(shift_image);
      return((Image *) NULL);
    }
  /*
    Blue-shift DirectClass image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  shift_view=AcquireAuthenticCacheView(shift_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,shift_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    MagickBooleanType
      sync;

    MagickPixelPacket
      pixel;

    Quantum
      quantum;

    const PixelPacket
      *magick_restrict p;

    ssize_t
      x;

    PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(shift_view,0,y,shift_image->columns,1,
      exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      quantum=GetPixelRed(p);
      if (GetPixelGreen(p) < quantum)
        quantum=GetPixelGreen(p);
      if (GetPixelBlue(p) < quantum)
        quantum=GetPixelBlue(p);
      pixel.red=0.5*(GetPixelRed(p)+factor*quantum);
      pixel.green=0.5*(GetPixelGreen(p)+factor*quantum);
      pixel.blue=0.5*(GetPixelBlue(p)+factor*quantum);
      quantum=GetPixelRed(p);
      if (GetPixelGreen(p) > quantum)
        quantum=GetPixelGreen(p);
      if (GetPixelBlue(p) > quantum)
        quantum=GetPixelBlue(p);
      pixel.red=0.5*(pixel.red+factor*quantum);
      pixel.green=0.5*(pixel.green+factor*quantum);
      pixel.blue=0.5*(pixel.blue+factor*quantum);
      SetPixelRed(q,ClampToQuantum(pixel.red));
      SetPixelGreen(q,ClampToQuantum(pixel.green));
      SetPixelBlue(q,ClampToQuantum(pixel.blue));
      p++;
      q++;
    }
    sync=SyncCacheViewAuthenticPixels(shift_view,exception);
    if (sync == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,BlueShiftImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  shift_view=DestroyCacheView(shift_view);
  if (status == MagickFalse)
    shift_image=DestroyImage(shift_image);
  return(shift_image);
}