static MagickBooleanType WriteWEBPImage(const ImageInfo *image_info,
  Image *image)
{
  const char
    *value;

  int
    webp_status;

  MagickBooleanType
    status;

  WebPAuxStats
    statistics;

  WebPConfig
    configure;

#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  WebPMemoryWriter
    writer_info;
#endif

  WebPPicture
    picture;

  PictureMemory
    memory = {0};

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if ((image->columns > 16383UL) || (image->rows > 16383UL))
    ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(status);
  if (WebPConfigInit(&configure) == 0)
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
  if (WebPPictureInit(&picture) == 0)
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");
#if !defined(MAGICKCORE_WEBPMUX_DELEGATE)
  picture.writer=WebPEncodeWriter;
  picture.custom_ptr=(void *) image;
#else
  WebPMemoryWriterInit(&writer_info);
  picture.writer=WebPMemoryWrite;
  picture.custom_ptr=(&writer_info);
#endif
  picture.stats=(&statistics);
  if (image->quality != UndefinedCompressionQuality)
    configure.quality=(float) image->quality;
  if (image->quality >= 100)
    configure.lossless=1;
  value=GetImageOption(image_info,"webp:lossless");
  if (value != (char *) NULL)
    configure.lossless=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:method");
  if (value != (char *) NULL)
    configure.method=StringToInteger(value);
  value=GetImageOption(image_info,"webp:image-hint");
  if (value != (char *) NULL)
    {
      if (LocaleCompare(value,"default") == 0)
        configure.image_hint=WEBP_HINT_DEFAULT;
      if (LocaleCompare(value,"photo") == 0)
        configure.image_hint=WEBP_HINT_PHOTO;
      if (LocaleCompare(value,"picture") == 0)
        configure.image_hint=WEBP_HINT_PICTURE;
#if WEBP_ENCODER_ABI_VERSION >= 0x0200
      if (LocaleCompare(value,"graph") == 0)
        configure.image_hint=WEBP_HINT_GRAPH;
#endif
    }
  value=GetImageOption(image_info,"webp:target-size");
  if (value != (char *) NULL)
    configure.target_size=StringToInteger(value);
  value=GetImageOption(image_info,"webp:target-psnr");
  if (value != (char *) NULL)
    configure.target_PSNR=(float) StringToDouble(value,(char **) NULL);
  value=GetImageOption(image_info,"webp:segments");
  if (value != (char *) NULL)
    configure.segments=StringToInteger(value);
  value=GetImageOption(image_info,"webp:sns-strength");
  if (value != (char *) NULL)
    configure.sns_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-strength");
  if (value != (char *) NULL)
    configure.filter_strength=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-sharpness");
  if (value != (char *) NULL)
    configure.filter_sharpness=StringToInteger(value);
  value=GetImageOption(image_info,"webp:filter-type");
  if (value != (char *) NULL)
    configure.filter_type=StringToInteger(value);
  value=GetImageOption(image_info,"webp:auto-filter");
  if (value != (char *) NULL)
    configure.autofilter=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:alpha-compression");
  if (value != (char *) NULL)
    configure.alpha_compression=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-filtering");
  if (value != (char *) NULL)
    configure.alpha_filtering=StringToInteger(value);
  value=GetImageOption(image_info,"webp:alpha-quality");
  if (value != (char *) NULL)
    configure.alpha_quality=StringToInteger(value);
  value=GetImageOption(image_info,"webp:pass");
  if (value != (char *) NULL)
    configure.pass=StringToInteger(value);
  value=GetImageOption(image_info,"webp:show-compressed");
  if (value != (char *) NULL)
    configure.show_compressed=StringToInteger(value);
  value=GetImageOption(image_info,"webp:preprocessing");
  if (value != (char *) NULL)
    configure.preprocessing=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partitions");
  if (value != (char *) NULL)
    configure.partitions=StringToInteger(value);
  value=GetImageOption(image_info,"webp:partition-limit");
  if (value != (char *) NULL)
    configure.partition_limit=StringToInteger(value);
#if WEBP_ENCODER_ABI_VERSION >= 0x0201
  value=GetImageOption(image_info,"webp:emulate-jpeg-size");
  if (value != (char *) NULL)
    configure.emulate_jpeg_size=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:low-memory");
  if (value != (char *) NULL)
    configure.low_memory=(int) ParseCommandOption(MagickBooleanOptions,
      MagickFalse,value);
  value=GetImageOption(image_info,"webp:thread-level");
  if (value != (char *) NULL)
    configure.thread_level=StringToInteger(value);
#endif
#if WEBP_ENCODER_ABI_VERSION >= 0x020e
  value=GetImageOption(image_info,"webp:use-sharp-yuv");
  if (value != (char *) NULL)
    configure.use_sharp_yuv=StringToInteger(value);
#endif
  if (WebPValidateConfig(&configure) == 0)
    ThrowWriterException(ResourceLimitError,"UnableToEncodeImageFile");

  WriteSingleWEBPImage(image_info,image,&picture,&memory,&image->exception);

#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  if ((image_info->adjoin != MagickFalse) &&
      (GetPreviousImageInList(image) == (Image *) NULL) &&
      (GetNextImageInList(image) != (Image *) NULL) &&
      (image->iterations != 1))
    WriteAnimatedWEBPImage(image_info,image,&configure,&writer_info,&image->exception);
#endif

  webp_status=WebPEncode(&configure,&picture);
  if (webp_status == 0)
    {
      const char
        *message;

      switch (picture.error_code)
      {
        case VP8_ENC_ERROR_OUT_OF_MEMORY:
        {
          message="out of memory";
          break;
        }
        case VP8_ENC_ERROR_BITSTREAM_OUT_OF_MEMORY:
        {
          message="bitstream out of memory";
          break;
        }
        case VP8_ENC_ERROR_NULL_PARAMETER:
        {
          message="NULL parameter";
          break;
        }
        case VP8_ENC_ERROR_INVALID_CONFIGURATION:
        {
          message="invalid configuration";
          break;
        }
        case VP8_ENC_ERROR_BAD_DIMENSION:
        {
          message="bad dimension";
          break;
        }
        case VP8_ENC_ERROR_PARTITION0_OVERFLOW:
        {
          message="partition 0 overflow (> 512K)";
          break;
        }
        case VP8_ENC_ERROR_PARTITION_OVERFLOW:
        {
          message="partition overflow (> 16M)";
          break;
        }
        case VP8_ENC_ERROR_BAD_WRITE:
        {
          message="bad write";
          break;
        }
        case VP8_ENC_ERROR_FILE_TOO_BIG:
        {
          message="file too big (> 4GB)";
          break;
        }
#if WEBP_ENCODER_ABI_VERSION >= 0x0100
        case VP8_ENC_ERROR_USER_ABORT:
        {
          message="user abort";
          break;
        }
#endif
        default:
        {
          message="unknown exception";
          break;
        }
      }
      (void) ThrowMagickException(&image->exception,GetMagickModule(),CorruptImageError,
        (char *) message,"`%s'",image->filename);
    }
#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  {
    const StringInfo
      *profile;

    WebPData
      chunk,
      image_chunk;

    WebPMux
      *mux;

    WebPMuxError
      mux_error;

    /*
      Set image profiles (if any).
    */
    image_chunk.bytes=writer_info.mem;
    image_chunk.size=writer_info.size;
    mux_error=WEBP_MUX_OK;
    (void) memset(&chunk,0,sizeof(chunk));
    mux=WebPMuxNew();
    profile=GetImageProfile(image,"ICC");
    if ((profile != (StringInfo *) NULL) && (mux_error == WEBP_MUX_OK))
      {
        chunk.bytes=GetStringInfoDatum(profile);
        chunk.size=GetStringInfoLength(profile);
        mux_error=WebPMuxSetChunk(mux,"ICCP",&chunk,0);
      }
    profile=GetImageProfile(image,"EXIF");
    if ((profile != (StringInfo *) NULL) && (mux_error == WEBP_MUX_OK))
      {
        chunk.bytes=GetStringInfoDatum(profile);
        chunk.size=GetStringInfoLength(profile);
        if ((chunk.size >= 6) &&
            (chunk.bytes[0] == 'E') && (chunk.bytes[1] == 'x') &&
            (chunk.bytes[2] == 'i') && (chunk.bytes[3] == 'f') &&
            (chunk.bytes[4] == '\0') && (chunk.bytes[5] == '\0'))
          {
            chunk.bytes=GetStringInfoDatum(profile)+6;
            chunk.size-=6;
          }
        mux_error=WebPMuxSetChunk(mux,"EXIF",&chunk,0);
      }
    profile=GetImageProfile(image,"XMP");
    if ((profile != (StringInfo *) NULL) && (mux_error == WEBP_MUX_OK))
      {
        chunk.bytes=GetStringInfoDatum(profile);
        chunk.size=GetStringInfoLength(profile);
        mux_error=WebPMuxSetChunk(mux,"XMP",&chunk,0);
      }
    if (mux_error != WEBP_MUX_OK)
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        ResourceLimitError,"UnableToEncodeImageFile","`%s'",image->filename);
    if (chunk.size != 0)
      {
        WebPData
          picture_profiles;

        /*
          Replace original container with image profile (if any).
        */
        picture_profiles.bytes=writer_info.mem;
        picture_profiles.size=writer_info.size;
        WebPMuxSetImage(mux,&image_chunk,1);
        mux_error=WebPMuxAssemble(mux,&picture_profiles);
        WebPMemoryWriterClear(&writer_info);
        writer_info.size=picture_profiles.size;
        writer_info.mem=(unsigned char *) picture_profiles.bytes;
      }
    WebPMuxDelete(mux);
  }
  (void) WriteBlob(image,writer_info.size,writer_info.mem);
#endif
  picture.argb=(uint32_t *) NULL;
  WebPPictureFree(&picture);
#if defined(MAGICKCORE_WEBPMUX_DELEGATE)
  WebPMemoryWriterClear(&writer_info);
#endif
  (void) CloseBlob(image);
  RelinquishVirtualMemory(memory.pixel_info);
  return(webp_status == 0 ? MagickFalse : MagickTrue);
}