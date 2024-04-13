MagickExport ResampleFilter *AcquireResampleFilter(const Image *image,
  ExceptionInfo *exception)
{
  ResampleFilter
    *resample_filter;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  resample_filter=(ResampleFilter *) AcquireCriticalMemory(sizeof(
    *resample_filter));
  (void) memset(resample_filter,0,sizeof(*resample_filter));
  resample_filter->exception=exception;
  resample_filter->image=ReferenceImage((Image *) image);
  resample_filter->view=AcquireVirtualCacheView(resample_filter->image,
    exception);
  resample_filter->debug=IsEventLogging();
  resample_filter->image_area=(ssize_t) (image->columns*image->rows);
  resample_filter->average_defined=MagickFalse;
  resample_filter->signature=MagickCoreSignature;
  SetResampleFilter(resample_filter,image->filter);
  (void) SetResampleFilterInterpolateMethod(resample_filter,image->interpolate);
  (void) SetResampleFilterVirtualPixelMethod(resample_filter,
    GetImageVirtualPixelMethod(image));
  return(resample_filter);
}