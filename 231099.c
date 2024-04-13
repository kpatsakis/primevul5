MagickExport MagickBooleanType SetImageBackgroundColor(Image *image)
{
  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  IndexPacket
    index;

  MagickBooleanType
    status;

  MagickPixelPacket
    background;

  PixelPacket
    pixel;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (SetImageStorageClass(image,DirectClass) == MagickFalse)
    return(MagickFalse);
  if ((IsPixelGray(&image->background_color) == MagickFalse) &&
      (IsGrayColorspace(image->colorspace) != MagickFalse))
    (void) TransformImageColorspace(image,RGBColorspace);
  if ((image->background_color.opacity != OpaqueOpacity) &&
      (image->matte == MagickFalse))
    (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
  GetMagickPixelPacket(image,&background);
  SetMagickPixelPacket(image,&image->background_color,(const IndexPacket *)
    NULL,&background);
  if (image->colorspace == CMYKColorspace)
    ConvertRGBToCMYK(&background);
  index=0;
  pixel.opacity=OpaqueOpacity;
  SetPixelPacket(image,&background,&pixel,&index);
  /*
    Set image background color.
  */
  status=MagickTrue;
  exception=(&image->exception);
  image_view=AcquireAuthenticCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
      *q++=pixel;
    if (image->colorspace == CMYKColorspace)
      {
        register IndexPacket
          *magick_restrict indexes;

        indexes=GetCacheViewAuthenticIndexQueue(image_view);
        for (x=0; x < (ssize_t) image->columns; x++)
          SetPixelIndex(indexes+x,index);
      }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}