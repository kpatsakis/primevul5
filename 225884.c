MagickExport Image *ShadowImage(const Image *image,const double opacity,
  const double sigma,const ssize_t x_offset,const ssize_t y_offset,
  ExceptionInfo *exception)
{
#define ShadowImageTag  "Shadow/Image"

  CacheView
    *image_view;

  Image
    *border_image,
    *clone_image,
    *shadow_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  RectangleInfo
    border_info;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  clone_image=CloneImage(image,0,0,MagickTrue,exception);
  if (clone_image == (Image *) NULL)
    return((Image *) NULL);
  if (IsGrayColorspace(image->colorspace) != MagickFalse)
    (void) SetImageColorspace(clone_image,sRGBColorspace);
  (void) SetImageVirtualPixelMethod(clone_image,EdgeVirtualPixelMethod);
  clone_image->compose=OverCompositeOp;
  border_info.width=(size_t) floor(2.0*sigma+0.5);
  border_info.height=(size_t) floor(2.0*sigma+0.5);
  border_info.x=0;
  border_info.y=0;
  (void) QueryColorDatabase("none",&clone_image->border_color,exception);
  border_image=BorderImage(clone_image,&border_info,exception);
  clone_image=DestroyImage(clone_image);
  if (border_image == (Image *) NULL)
    return((Image *) NULL);
  if (border_image->matte == MagickFalse)
    (void) SetImageAlphaChannel(border_image,OpaqueAlphaChannel);
  /*
    Shadow image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(border_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(border_image,border_image,border_image->rows,1)
#endif
  for (y=0; y < (ssize_t) border_image->rows; y++)
  {
    PixelPacket
      *magick_restrict q;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,border_image->columns,1,
      exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) border_image->columns; x++)
    {
      SetPixelRed(q,border_image->background_color.red);
      SetPixelGreen(q,border_image->background_color.green);
      SetPixelBlue(q,border_image->background_color.blue);
      if (border_image->matte == MagickFalse)
        SetPixelOpacity(q,border_image->background_color.opacity);
      else
        SetPixelOpacity(q,ClampToQuantum((double) (QuantumRange-
          GetPixelAlpha(q)*opacity/100.0)));
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,ShadowImageTag,progress,
          border_image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  shadow_image=BlurImageChannel(border_image,AlphaChannel,0.0,sigma,exception);
  border_image=DestroyImage(border_image);
  if (shadow_image == (Image *) NULL)
    return((Image *) NULL);
  if (shadow_image->page.width == 0)
    shadow_image->page.width=shadow_image->columns;
  if (shadow_image->page.height == 0)
    shadow_image->page.height=shadow_image->rows;
  shadow_image->page.width+=x_offset-(ssize_t) border_info.width;
  shadow_image->page.height+=y_offset-(ssize_t) border_info.height;
  shadow_image->page.x+=x_offset-(ssize_t) border_info.width;
  shadow_image->page.y+=y_offset-(ssize_t) border_info.height;
  return(shadow_image);
}