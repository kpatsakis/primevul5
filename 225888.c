MagickExport Image *CharcoalImage(const Image *image,const double radius,
  const double sigma,ExceptionInfo *exception)
{
  Image
    *charcoal_image,
    *edge_image;

  MagickBooleanType
    status;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  edge_image=EdgeImage(image,radius,exception);
  if (edge_image == (Image *) NULL)
    return((Image *) NULL);
  charcoal_image=(Image *) NULL;
  status=ClampImage(edge_image);
  if (status != MagickFalse)
    charcoal_image=BlurImage(edge_image,radius,sigma,exception);
  edge_image=DestroyImage(edge_image);
  if (charcoal_image == (Image *) NULL)
    return((Image *) NULL);
  status=NormalizeImage(charcoal_image);
  if (status != MagickFalse)
    status=NegateImage(charcoal_image,MagickFalse);
  if (status != MagickFalse)
    status=GrayscaleImage(charcoal_image,image->intensity);
  if (status == MagickFalse)
    charcoal_image=DestroyImage(charcoal_image);
  return(charcoal_image);
}