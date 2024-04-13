MagickExport Image *ColorizeImage(const Image *image,const char *blend,
  const PixelInfo *colorize,ExceptionInfo *exception)
{
#define ColorizeImageTag  "Colorize/Image"
#define Colorize(pixel,blend_percentage,colorize)  \
  (((pixel)*(100.0-(blend_percentage))+(colorize)*(blend_percentage))/100.0)

  CacheView
    *image_view;

  GeometryInfo
    geometry_info;

  Image
    *colorize_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  MagickStatusType
    flags;

  PixelInfo
    blend_percentage;

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
  if (SetImageStorageClass(colorize_image,DirectClass,exception) == MagickFalse)
    {
      colorize_image=DestroyImage(colorize_image);
      return((Image *) NULL);
    }
  if ((IsGrayColorspace(colorize_image->colorspace) != MagickFalse) ||
      (IsPixelInfoGray(colorize) != MagickFalse))
    (void) SetImageColorspace(colorize_image,sRGBColorspace,exception);
  if ((colorize_image->alpha_trait == UndefinedPixelTrait) &&
      (colorize->alpha_trait != UndefinedPixelTrait))
    (void) SetImageAlpha(colorize_image,OpaqueAlpha,exception);
  if (blend == (const char *) NULL)
    return(colorize_image);
  GetPixelInfo(colorize_image,&blend_percentage);
  flags=ParseGeometry(blend,&geometry_info);
  blend_percentage.red=geometry_info.rho;
  blend_percentage.green=geometry_info.rho;
  blend_percentage.blue=geometry_info.rho;
  blend_percentage.black=geometry_info.rho;
  blend_percentage.alpha=(MagickRealType) TransparentAlpha;
  if ((flags & SigmaValue) != 0)
    blend_percentage.green=geometry_info.sigma;
  if ((flags & XiValue) != 0)
    blend_percentage.blue=geometry_info.xi;
  if ((flags & PsiValue) != 0)
    blend_percentage.alpha=geometry_info.psi;
  if (blend_percentage.colorspace == CMYKColorspace)
    {
      if ((flags & PsiValue) != 0)
        blend_percentage.black=geometry_info.psi;
      if ((flags & ChiValue) != 0)
        blend_percentage.alpha=geometry_info.chi;
    }
  /*
    Colorize DirectClass image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(colorize_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(colorize_image,colorize_image,colorize_image->rows,1)
#endif
  for (y=0; y < (ssize_t) colorize_image->rows; y++)
  {
    MagickBooleanType
      sync;

    Quantum
      *magick_restrict q;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,colorize_image->columns,1,
      exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) colorize_image->columns; x++)
    {
      ssize_t
        i;

      for (i=0; i < (ssize_t) GetPixelChannels(colorize_image); i++)
      {
        PixelTrait traits = GetPixelChannelTraits(colorize_image,
          (PixelChannel) i);
        if (traits == UndefinedPixelTrait)
          continue;
        if ((traits & CopyPixelTrait) != 0)
          continue;
        SetPixelChannel(colorize_image,(PixelChannel) i,ClampToQuantum(
          Colorize(q[i],GetPixelInfoChannel(&blend_percentage,(PixelChannel) i),
          GetPixelInfoChannel(colorize,(PixelChannel) i))),q);
      }
      q+=GetPixelChannels(colorize_image);
    }
    sync=SyncCacheViewAuthenticPixels(image_view,exception);
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
        proceed=SetImageProgress(image,ColorizeImageTag,progress,
          colorize_image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    colorize_image=DestroyImage(colorize_image);
  return(colorize_image);
}