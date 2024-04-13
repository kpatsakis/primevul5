static MagickBooleanType SerializeImageChannel(const ImageInfo *image_info,
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

  size_t
    pack,
    padded_columns;

  ssize_t
    y;

  unsigned char
    code,
    bit;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=MagickTrue;
  pack=SetImageMonochrome(image,exception) == MagickFalse ? 1UL : 8UL;
  padded_columns=((image->columns+pack-1)/pack)*pack;
  *length=(size_t) padded_columns*image->rows/pack;
  *pixel_info=AcquireVirtualMemory(*length,sizeof(*q));
  if (*pixel_info == (MemoryInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  q=(unsigned char *) GetVirtualMemoryBlob(*pixel_info);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    if (pack == 1)
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        *q++=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        p+=GetPixelChannels(image);
      }
    else
      {
        code='\0';
        for (x=0; x < (ssize_t) padded_columns; x++)
        {
          bit=(unsigned char) 0x00;
          if (x < (ssize_t) image->columns)
            bit=(unsigned char) (GetPixelLuma(image,p) == TransparentAlpha ?
              0x01 : 0x00);
          code=(code << 1)+bit;
          if (((x+1) % pack) == 0)
            {
              *q++=code;
              code='\0';
            }
          p+=GetPixelChannels(image);
        }
      }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  if (status == MagickFalse)
    *pixel_info=RelinquishVirtualMemory(*pixel_info);
  return(status);
}