MagickExport MagickBooleanType SetResampleFilterVirtualPixelMethod(
  ResampleFilter *resample_filter,const VirtualPixelMethod method)
{
  assert(resample_filter != (ResampleFilter *) NULL);
  assert(resample_filter->signature == MagickCoreSignature);
  assert(resample_filter->image != (Image *) NULL);
  if (resample_filter->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      resample_filter->image->filename);
  resample_filter->virtual_pixel=method;
  if (method != UndefinedVirtualPixelMethod)
    (void) SetCacheViewVirtualPixelMethod(resample_filter->view,method);
  return(MagickTrue);
}