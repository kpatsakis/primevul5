MagickExport Image *ColorizeImage(const Image *image,const char *opacity,
  const PixelPacket colorize,ExceptionInfo *exception)
{
#define ColorizeImageTag  "Colorize/Image"

  CacheView
    *colorize_view,
    *image_view;

  GeometryInfo
    geometry_info;

  Image
    *colorize_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    pixel;

  MagickStatusType
    flags;

  ssize_t
    y;

  /*
    Allocate colorized image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  colorize_image=CloneImage(image,0,0,MagickTrue,exception);
  if (colorize_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(colorize_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&colorize_image->exception);
      colorize_image=DestroyImage(colorize_image);
      return((Image *) NULL);
    }
  if ((IsGrayColorspace(image->colorspace) != MagickFalse) ||
      (IsPixelGray(&colorize) != MagickFalse))
    (void) SetImageColorspace(colorize_image,sRGBColorspace);
  if ((colorize_image->matte == MagickFalse) &&
      (colorize.opacity != OpaqueOpacity))
    (void) SetImageAlphaChannel(colorize_image,OpaqueAlphaChannel);
  if (opacity == (const char *) NULL)
    return(colorize_image);
  /*
    Determine RGB values of the pen color.
  */
  flags=ParseGeometry(opacity,&geometry_info);
  pixel.red=geometry_info.rho;
  pixel.green=geometry_info.rho;
  pixel.blue=geometry_info.rho;
  pixel.opacity=(MagickRealType) OpaqueOpacity;
  if ((flags & SigmaValue) != 0)
    pixel.green=geometry_info.sigma;
  if ((flags & XiValue) != 0)
    pixel.blue=geometry_info.xi;
  if ((flags & PsiValue) != 0)
    pixel.opacity=geometry_info.psi;
  /*
    Colorize DirectClass image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  colorize_view=AcquireAuthenticCacheView(colorize_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,colorize_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    MagickBooleanType
      sync;

    const PixelPacket
      *magick_restrict p;

    ssize_t
      x;

    PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(colorize_view,0,y,colorize_image->columns,1,
      exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      SetPixelRed(q,((GetPixelRed(p)*(100.0-pixel.red)+
        colorize.red*pixel.red)/100.0));
      SetPixelGreen(q,((GetPixelGreen(p)*(100.0-pixel.green)+
        colorize.green*pixel.green)/100.0));
      SetPixelBlue(q,((GetPixelBlue(p)*(100.0-pixel.blue)+
        colorize.blue*pixel.blue)/100.0));
      if (colorize_image->matte == MagickFalse)
        SetPixelOpacity(q,GetPixelOpacity(p));
      else
        SetPixelOpacity(q,((GetPixelOpacity(p)*(100.0-pixel.opacity)+
          colorize.opacity*pixel.opacity)/100.0));
      p++;
      q++;
    }
    sync=SyncCacheViewAuthenticPixels(colorize_view,exception);
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
        proceed=SetImageProgress(image,ColorizeImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  colorize_view=DestroyCacheView(colorize_view);
  if (status == MagickFalse)
    colorize_image=DestroyImage(colorize_image);
  return(colorize_image);
}