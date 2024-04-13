MagickExport Image *TintImage(const Image *image,const char *opacity,
  const PixelPacket tint,ExceptionInfo *exception)
{
#define TintImageTag  "Tint/Image"

  CacheView
    *image_view,
    *tint_view;

  GeometryInfo
    geometry_info;

  Image
    *tint_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickPixelPacket
    color_vector,
    pixel;

  MagickStatusType
    flags;

  ssize_t
    y;

  /*
    Allocate tint image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  tint_image=CloneImage(image,0,0,MagickTrue,exception);
  if (tint_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(tint_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&tint_image->exception);
      tint_image=DestroyImage(tint_image);
      return((Image *) NULL);
    }
  if ((IsGrayColorspace(image->colorspace) != MagickFalse) &&
      (IsPixelGray(&tint) == MagickFalse))
    (void) SetImageColorspace(tint_image,sRGBColorspace);
  if (opacity == (const char *) NULL)
    return(tint_image);
  /*
    Determine RGB values of the tint color.
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
  color_vector.red=(MagickRealType) (pixel.red*tint.red/100.0-
    PixelPacketIntensity(&tint));
  color_vector.green=(MagickRealType) (pixel.green*tint.green/100.0-
    PixelPacketIntensity(&tint));
  color_vector.blue=(MagickRealType) (pixel.blue*tint.blue/100.0-
    PixelPacketIntensity(&tint));
  /*
    Tint image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  tint_view=AcquireAuthenticCacheView(tint_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,tint_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const PixelPacket
      *magick_restrict p;

    PixelPacket
      *magick_restrict q;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(tint_view,0,y,tint_image->columns,1,
      exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      double
        weight;

      MagickPixelPacket
        pixel;

      weight=QuantumScale*GetPixelRed(p)-0.5;
      pixel.red=(MagickRealType) GetPixelRed(p)+color_vector.red*(1.0-(4.0*
        (weight*weight)));
      SetPixelRed(q,ClampToQuantum(pixel.red));
      weight=QuantumScale*GetPixelGreen(p)-0.5;
      pixel.green=(MagickRealType) GetPixelGreen(p)+color_vector.green*(1.0-
        (4.0*(weight*weight)));
      SetPixelGreen(q,ClampToQuantum(pixel.green));
      weight=QuantumScale*GetPixelBlue(p)-0.5;
      pixel.blue=(MagickRealType) GetPixelBlue(p)+color_vector.blue*(1.0-(4.0*
        (weight*weight)));
      SetPixelBlue(q,ClampToQuantum(pixel.blue));
      SetPixelOpacity(q,GetPixelOpacity(p));
      p++;
      q++;
    }
    if (SyncCacheViewAuthenticPixels(tint_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,TintImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  tint_view=DestroyCacheView(tint_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    tint_image=DestroyImage(tint_image);
  return(tint_image);
}