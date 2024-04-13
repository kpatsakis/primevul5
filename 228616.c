static MagickBooleanType SerializeImageIndexes(const ImageInfo *image_info,
  Image *image,MemoryInfo **pixel_info,size_t *length,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  register const Quantum
    *p;

  register ssize_t
    x;

  register unsigned char
    *q;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=MagickTrue;
  *length=(size_t) image->columns*image->rows;
  *pixel_info=AcquireVirtualMemory(*length,sizeof(*q));
  if (*pixel_info == (MemoryInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  q=(unsigned char *) GetVirtualMemoryBlob(*pixel_info);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      *q++=(unsigned char) GetPixelIndex(image,p);
      p+=GetPixelChannels(image);
    }
    if (image->previous == (Image *) NULL)
      {
        status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
          image->rows);
        if (status == MagickFalse)
          break;
      }
  }
  if (status == MagickFalse)
    *pixel_info=RelinquishVirtualMemory(*pixel_info);
  return(status);
}