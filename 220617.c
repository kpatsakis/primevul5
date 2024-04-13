MagickExport Image *TintImage(const Image *image,const char *blend,
  const PixelInfo *tint,ExceptionInfo *exception)
{
#define TintImageTag  "Tint/Image"

  CacheView
    *image_view,
    *tint_view;

  double
    intensity;

  GeometryInfo
    geometry_info;

  Image
    *tint_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PixelInfo
    color_vector;

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
  if (SetImageStorageClass(tint_image,DirectClass,exception) == MagickFalse)
    {
      tint_image=DestroyImage(tint_image);
      return((Image *) NULL);
    }
  if ((IsGrayColorspace(image->colorspace) != MagickFalse) &&
      (IsPixelInfoGray(tint) == MagickFalse))
    (void) SetImageColorspace(tint_image,sRGBColorspace,exception);
  if (blend == (const char *) NULL)
    return(tint_image);
  /*
    Determine RGB values of the color.
  */
  GetPixelInfo(image,&color_vector);
  flags=ParseGeometry(blend,&geometry_info);
  color_vector.red=geometry_info.rho;
  color_vector.green=geometry_info.rho;
  color_vector.blue=geometry_info.rho;
  color_vector.alpha=(MagickRealType) OpaqueAlpha;
  if ((flags & SigmaValue) != 0)
    color_vector.green=geometry_info.sigma;
  if ((flags & XiValue) != 0)
    color_vector.blue=geometry_info.xi;
  if ((flags & PsiValue) != 0)
    color_vector.alpha=geometry_info.psi;
  if (image->colorspace == CMYKColorspace)
    {
      color_vector.black=geometry_info.rho;
      if ((flags & PsiValue) != 0)
        color_vector.black=geometry_info.psi;
      if ((flags & ChiValue) != 0)
        color_vector.alpha=geometry_info.chi;
    }
  intensity=(double) GetPixelInfoIntensity((const Image *) NULL,tint);
  color_vector.red=(double) (color_vector.red*tint->red/100.0-intensity);
  color_vector.green=(double) (color_vector.green*tint->green/100.0-intensity);
  color_vector.blue=(double) (color_vector.blue*tint->blue/100.0-intensity);
  color_vector.black=(double) (color_vector.black*tint->black/100.0-intensity);
  color_vector.alpha=(double) (color_vector.alpha*tint->alpha/100.0-intensity);
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
    const Quantum
      *magick_restrict p;

    Quantum
      *magick_restrict q;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=QueueCacheViewAuthenticPixels(tint_view,0,y,tint_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      PixelInfo
        pixel;

      double
        weight;

      GetPixelInfo(image,&pixel);
      weight=QuantumScale*GetPixelRed(image,p)-0.5;
      pixel.red=(MagickRealType) GetPixelRed(image,p)+color_vector.red*
        (1.0-(4.0*(weight*weight)));
      weight=QuantumScale*GetPixelGreen(image,p)-0.5;
      pixel.green=(MagickRealType) GetPixelGreen(image,p)+color_vector.green*
        (1.0-(4.0*(weight*weight)));
      weight=QuantumScale*GetPixelBlue(image,p)-0.5;
      pixel.blue=(MagickRealType) GetPixelBlue(image,p)+color_vector.blue*
        (1.0-(4.0*(weight*weight)));
      weight=QuantumScale*GetPixelBlack(image,p)-0.5;
      pixel.black=(MagickRealType) GetPixelBlack(image,p)+color_vector.black*
        (1.0-(4.0*(weight*weight)));
      pixel.alpha=(MagickRealType) GetPixelAlpha(image,p);
      SetPixelViaPixelInfo(tint_image,&pixel,q);
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(tint_image);
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