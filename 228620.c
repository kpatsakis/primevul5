static MagickBooleanType WritePS3MaskImage(const ImageInfo *image_info,
  Image *image,const CompressionType compression,ExceptionInfo *exception)
{
  char
    buffer[MagickPathExtent];

  Image
    *mask_image;

  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    start,
    stop;

  MemoryInfo
    *pixel_info;

  register ssize_t
    i;

  size_t
    length;

  unsigned char
    *pixels;

  assert(image_info != (ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->alpha_trait != UndefinedPixelTrait);
  status=MagickTrue;
  /*
    Note BeginData DSC comment for update later.
  */
  start=TellBlob(image);
  if (start < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  (void) FormatLocaleString(buffer,MagickPathExtent,
    "%%%%BeginData:%13ld %s Bytes\n",0L,compression == NoCompression ?
    "ASCII" : "BINARY");
  (void) WriteBlobString(image,buffer);
  stop=TellBlob(image);
  if (stop < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  /*
    Only lossless compressions for the mask.
  */
  switch (compression)
  {
    case NoCompression:
    default:
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,
        "currentfile %.20g %.20g " PS3_NoCompression
        " ByteStreamDecodeFilter\n",(double) image->columns,(double)
        image->rows);
      break;
    }
    case FaxCompression:
    case Group4Compression:
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,
        "currentfile %.20g %.20g " PS3_FaxCompression
        " ByteStreamDecodeFilter\n",(double) image->columns,(double)
        image->rows);
      break;
    }
    case LZWCompression:
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,
        "currentfile %.20g %.20g " PS3_LZWCompression
        " ByteStreamDecodeFilter\n",(double) image->columns,(double)
        image->rows);
      break;
    }
    case RLECompression:
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,
        "currentfile %.20g %.20g " PS3_RLECompression
        " ByteStreamDecodeFilter\n",(double) image->columns,(double)
        image->rows);
      break;
    }
    case ZipCompression:
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,
        "currentfile %.20g %.20g " PS3_ZipCompression
        " ByteStreamDecodeFilter\n",(double) image->columns,(double)
        image->rows);
      break;
    }
  }
  (void) WriteBlobString(image,buffer);
  (void) WriteBlobString(image,"/ReusableStreamDecode filter\n");
  mask_image=SeparateImage(image,AlphaChannel,exception);
  if (mask_image == (Image *) NULL)
    ThrowWriterException(CoderError,exception->reason);
  (void) SetImageType(mask_image,BilevelType,exception);
  (void) SetImageType(mask_image,PaletteType,exception);
  mask_image->alpha_trait=UndefinedPixelTrait;
  pixels=(unsigned char *) NULL;
  length=0;
  switch (compression)
  {
    case NoCompression:
    default:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length,
        exception);
      if (status == MagickFalse)
        break;
      Ascii85Initialize(image);
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      for (i=0; i < (ssize_t) length; i++)
        Ascii85Encode(image,pixels[i]);
      Ascii85Flush(image);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
    case FaxCompression:
    case Group4Compression:
    {
      if ((compression == FaxCompression) ||
          (LocaleCompare(CCITTParam,"0") == 0))
        status=HuffmanEncodeImage(image_info,image,mask_image,exception);
      else
        status=Huffman2DEncodeImage(image_info,image,mask_image,exception);
      break;
    }
    case LZWCompression:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length,
        exception);
      if (status == MagickFalse)
        break;
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      status=LZWEncodeImage(image,length,pixels,exception);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
    case RLECompression:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length,
        exception);
      if (status == MagickFalse)
        break;
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      status=PackbitsEncodeImage(image,length,pixels,exception);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
    case ZipCompression:
    {
      status=SerializeImageChannel(image_info,mask_image,&pixel_info,&length,
        exception);
      if (status == MagickFalse)
        break;
      pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
      status=ZLIBEncodeImage(image,length,pixels,exception);
      pixel_info=RelinquishVirtualMemory(pixel_info);
      break;
    }
  }
  mask_image=DestroyImage(mask_image);
  (void) WriteBlobByte(image,'\n');
  length=(size_t) (TellBlob(image)-stop);
  stop=TellBlob(image);
  if (stop < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  offset=SeekBlob(image,start,SEEK_SET);
  if (offset < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  (void) FormatLocaleString(buffer,MagickPathExtent,
    "%%%%BeginData:%13ld %s Bytes\n",(long) length,
    compression == NoCompression ? "ASCII" : "BINARY");
  (void) WriteBlobString(image,buffer);
  offset=SeekBlob(image,stop,SEEK_SET);
  if (offset < 0)
    ThrowWriterException(CorruptImageError,"ImproperImageHeader");
  (void) WriteBlobString(image,"%%EndData\n");
  (void) WriteBlobString(image, "/mask_stream exch def\n");
  return(status);
}