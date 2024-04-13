MagickExport ResampleFilter *DestroyResampleFilter(
  ResampleFilter *resample_filter)
{
  assert(resample_filter != (ResampleFilter *) NULL);
  assert(resample_filter->signature == MagickCoreSignature);
  assert(resample_filter->image != (Image *) NULL);
  if (resample_filter->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      resample_filter->image->filename);
  resample_filter->view=DestroyCacheView(resample_filter->view);
  resample_filter->image=DestroyImage(resample_filter->image);
#if ! FILTER_LUT
  resample_filter->filter_def=DestroyResizeFilter(resample_filter->filter_def);
#endif
  resample_filter->signature=(~MagickCoreSignature);
  resample_filter=(ResampleFilter *) RelinquishMagickMemory(resample_filter);
  return(resample_filter);
}