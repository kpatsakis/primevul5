MagickExport Image *VignetteImage(const Image *image,const double radius,
  const double sigma,const ssize_t x,const ssize_t y,ExceptionInfo *exception)
{
  char
    ellipse[MaxTextExtent];

  DrawInfo
    *draw_info;

  Image
    *blur_image,
    *canvas_image,
    *oval_image,
    *vignette_image;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  canvas_image=CloneImage(image,0,0,MagickTrue,exception);
  if (canvas_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(canvas_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&canvas_image->exception);
      canvas_image=DestroyImage(canvas_image);
      return((Image *) NULL);
    }
  canvas_image->matte=MagickTrue;
  oval_image=CloneImage(canvas_image,canvas_image->columns,canvas_image->rows,
    MagickTrue,exception);
  if (oval_image == (Image *) NULL)
    {
      canvas_image=DestroyImage(canvas_image);
      return((Image *) NULL);
    }
  (void) QueryColorDatabase("#000000",&oval_image->background_color,exception);
  (void) SetImageBackgroundColor(oval_image);
  draw_info=CloneDrawInfo((const ImageInfo *) NULL,(const DrawInfo *) NULL);
  (void) QueryColorDatabase("#ffffff",&draw_info->fill,exception);
  (void) QueryColorDatabase("#ffffff",&draw_info->stroke,exception);
  (void) FormatLocaleString(ellipse,MaxTextExtent,
    "ellipse %g,%g,%g,%g,0.0,360.0",image->columns/2.0,
    image->rows/2.0,image->columns/2.0-x,image->rows/2.0-y);
  draw_info->primitive=AcquireString(ellipse);
  (void) DrawImage(oval_image,draw_info);
  draw_info=DestroyDrawInfo(draw_info);
  blur_image=BlurImage(oval_image,radius,sigma,exception);
  oval_image=DestroyImage(oval_image);
  if (blur_image == (Image *) NULL)
    {
      canvas_image=DestroyImage(canvas_image);
      return((Image *) NULL);
    }
  blur_image->matte=MagickFalse;
  (void) CompositeImage(canvas_image,CopyOpacityCompositeOp,blur_image,0,0);
  blur_image=DestroyImage(blur_image);
  vignette_image=MergeImageLayers(canvas_image,FlattenLayer,exception);
  canvas_image=DestroyImage(canvas_image);
  if (vignette_image != (Image *) NULL)
    (void) TransformImageColorspace(vignette_image,image->colorspace);
  return(vignette_image);
}