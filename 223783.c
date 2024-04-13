static Image *ReadWEBPImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define ThrowWEBPException(severity,tag) \
{ \
  if (stream != (unsigned char *) NULL) \
    stream=(unsigned char*) RelinquishMagickMemory(stream); \
  if (webp_image != (WebPDecBuffer *) NULL) \
    WebPFreeDecBuffer(webp_image); \
  ThrowReaderException(severity,tag); \
}

  Image
    *image;

  int
    webp_status;

  MagickBooleanType
    status;

  size_t
    length;

  ssize_t
    count;

  unsigned char
    header[12],
    *stream;

  WebPDecoderConfig
    configure;

  WebPDecBuffer
    *magick_restrict webp_image = &configure.output;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  stream=(unsigned char *) NULL;
  if (WebPInitDecoderConfig(&configure) == 0)
    ThrowReaderException(ResourceLimitError,"UnableToDecodeImageFile");
  webp_image->colorspace=MODE_RGBA;
  count=ReadBlob(image,12,header);
  if (count != 12)
    ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");
  status=IsWEBP(header,count);
  if (status == MagickFalse)
    ThrowWEBPException(CorruptImageError,"CorruptImage");
  length=(size_t) (ReadWebPLSBWord(header+4)+8);
  if (length < 12)
    ThrowWEBPException(CorruptImageError,"CorruptImage");
  if (length > GetBlobSize(image))
    ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");
  stream=(unsigned char *) AcquireQuantumMemory(length,sizeof(*stream));
  if (stream == (unsigned char *) NULL)
    ThrowWEBPException(ResourceLimitError,"MemoryAllocationFailed");
  (void) memcpy(stream,header,12);
  count=ReadBlob(image,length-12,stream+12);
  if (count != (ssize_t) (length-12))
    ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");

  webp_status=FillBasicWEBPInfo(image,stream,length,&configure);
  if (webp_status == VP8_STATUS_OK) {
    if (configure.input.has_animation) {
#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
      webp_status=ReadAnimatedWEBPImage(image_info,image,stream,length,
        &configure,exception);
#else
      webp_status=VP8_STATUS_UNSUPPORTED_FEATURE;
#endif
    } else {
      webp_status=ReadSingleWEBPImage(image,stream,length,&configure,exception,MagickFalse);
    }
  }

  if (webp_status != VP8_STATUS_OK)
    switch (webp_status)
    {
      case VP8_STATUS_OUT_OF_MEMORY:
      {
        ThrowWEBPException(ResourceLimitError,"MemoryAllocationFailed");
        break;
      }
      case VP8_STATUS_INVALID_PARAM:
      {
        ThrowWEBPException(CorruptImageError,"invalid parameter");
        break;
      }
      case VP8_STATUS_BITSTREAM_ERROR:
      {
        ThrowWEBPException(CorruptImageError,"CorruptImage");
        break;
      }
      case VP8_STATUS_UNSUPPORTED_FEATURE:
      {
        ThrowWEBPException(CoderError,"DataEncodingSchemeIsNotSupported");
        break;
      }
      case VP8_STATUS_SUSPENDED:
      {
        ThrowWEBPException(CorruptImageError,"decoder suspended");
        break;
      }
      case VP8_STATUS_USER_ABORT:
      {
        ThrowWEBPException(CorruptImageError,"user abort");
        break;
      }
      case VP8_STATUS_NOT_ENOUGH_DATA:
      {
        ThrowWEBPException(CorruptImageError,"InsufficientImageDataInFile");
        break;
      }
      default:
        ThrowWEBPException(CorruptImageError,"CorruptImage");
    }

  stream=(unsigned char*) RelinquishMagickMemory(stream);
  (void) CloseBlob(image);
  return(image);
}