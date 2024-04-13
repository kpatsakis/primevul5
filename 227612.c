MagickExport MagickBooleanType SetResampleFilterInterpolateMethod(
  ResampleFilter *resample_filter,const PixelInterpolateMethod method)
{
  assert(resample_filter != (ResampleFilter *) NULL);
  assert(resample_filter->signature == MagickCoreSignature);
  assert(resample_filter->image != (Image *) NULL);
  if (resample_filter->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      resample_filter->image->filename);
  resample_filter->interpolate=method;
  return(MagickTrue);
}