MagickExport Image *GetImageMask(const Image *image,ExceptionInfo *exception)
{
  assert(image != (const Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (image->mask == (Image *) NULL)
    return((Image *) NULL);
  return(CloneImage(image->mask,0,0,MagickTrue,exception));
}