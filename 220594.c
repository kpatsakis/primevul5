MagickExport Image *VignetteImage(const Image *image,const double radius,
  const double sigma,const ssize_t x,const ssize_t y,ExceptionInfo *exception)
{
  char
    ellipse[MagickPathExtent];

  DrawInfo
    *draw_info;

  Image
    *canvas,
    *blur_image,
    *oval_image,
    *vignette_image;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  canvas=CloneImage(image,0,0,MagickTrue,exception);
  if (canvas == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(canvas,DirectClass,exception) == MagickFalse)
    {
      canvas=DestroyImage(canvas);
      return((Image *) NULL);
    }
  canvas->alpha_trait=BlendPixelTrait;
  oval_image=CloneImage(canvas,canvas->columns,canvas->rows,MagickTrue,
    exception);
  if (oval_image == (Image *) NULL)
    {
      canvas=DestroyImage(canvas);
      return((Image *) NULL);
    }
  (void) QueryColorCompliance("#000000",AllCompliance,
    &oval_image->background_color,exception);
  (void) SetImageBackgroundColor(oval_image,exception);
  draw_info=CloneDrawInfo((const ImageInfo *) NULL,(const DrawInfo *) NULL);
  (void) QueryColorCompliance("#ffffff",AllCompliance,&draw_info->fill,
    exception);
  (void) QueryColorCompliance("#ffffff",AllCompliance,&draw_info->stroke,
    exception);
  (void) FormatLocaleString(ellipse,MagickPathExtent,"ellipse %g,%g,%g,%g,"
    "0.0,360.0",image->columns/2.0,image->rows/2.0,image->columns/2.0-x,
    image->rows/2.0-y);
  draw_info->primitive=AcquireString(ellipse);
  (void) DrawImage(oval_image,draw_info,exception);
  draw_info=DestroyDrawInfo(draw_info);
  blur_image=BlurImage(oval_image,radius,sigma,exception);
  oval_image=DestroyImage(oval_image);
  if (blur_image == (Image *) NULL)
    {
      canvas=DestroyImage(canvas);
      return((Image *) NULL);
    }
  blur_image->alpha_trait=UndefinedPixelTrait;
  (void) CompositeImage(canvas,blur_image,IntensityCompositeOp,MagickTrue,
    0,0,exception);
  blur_image=DestroyImage(blur_image);
  vignette_image=MergeImageLayers(canvas,FlattenLayer,exception);
  canvas=DestroyImage(canvas);
  if (vignette_image != (Image *) NULL)
    (void) TransformImageColorspace(vignette_image,image->colorspace,exception);
  return(vignette_image);
}