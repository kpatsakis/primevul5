MagickExport Image *ShadowImage(const Image *image,const double alpha,
  const double sigma,const ssize_t x_offset,const ssize_t y_offset,
  ExceptionInfo *exception)
{
#define ShadowImageTag  "Shadow/Image"

  CacheView
    *image_view;

  ChannelType
    channel_mask;

  Image
    *border_image,
    *clone_image,
    *shadow_image;

  MagickBooleanType
    status;

  PixelInfo
    background_color;

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
    (void) SetImageColorspace(clone_image,sRGBColorspace,exception);
  (void) SetImageVirtualPixelMethod(clone_image,EdgeVirtualPixelMethod,
    exception);
  border_info.width=(size_t) floor(2.0*sigma+0.5);
  border_info.height=(size_t) floor(2.0*sigma+0.5);
  border_info.x=0;
  border_info.y=0;
  (void) QueryColorCompliance("none",AllCompliance,&clone_image->border_color,
    exception);
  clone_image->alpha_trait=BlendPixelTrait;
  border_image=BorderImage(clone_image,&border_info,OverCompositeOp,exception);
  clone_image=DestroyImage(clone_image);
  if (border_image == (Image *) NULL)
    return((Image *) NULL);
  if (border_image->alpha_trait == UndefinedPixelTrait)
    (void) SetImageAlphaChannel(border_image,OpaqueAlphaChannel,exception);
  /*
    Shadow image.
  */
  status=MagickTrue;
  background_color=border_image->background_color;
  background_color.alpha_trait=BlendPixelTrait;
  image_view=AcquireAuthenticCacheView(border_image,exception);
  for (y=0; y < (ssize_t) border_image->rows; y++)
  {
    Quantum
      *magick_restrict q;

    ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(image_view,0,y,border_image->columns,1,
      exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) border_image->columns; x++)
    {
      if (border_image->alpha_trait != UndefinedPixelTrait)
        background_color.alpha=GetPixelAlpha(border_image,q)*alpha/100.0;
      SetPixelViaPixelInfo(border_image,&background_color,q);
      q+=GetPixelChannels(border_image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    {
      border_image=DestroyImage(border_image);
      return((Image *) NULL);
    }
  channel_mask=SetImageChannelMask(border_image,AlphaChannel);
  shadow_image=BlurImage(border_image,0.0,sigma,exception);
  border_image=DestroyImage(border_image);
  if (shadow_image == (Image *) NULL)
    return((Image *) NULL);
  (void) SetPixelChannelMask(shadow_image,channel_mask);
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