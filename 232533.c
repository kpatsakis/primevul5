static MagickBooleanType WriteHEICImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  ssize_t
    y;

  struct heif_context
    *heif_context;

  struct heif_encoder
    *heif_encoder;

  struct heif_image
    *heif_image;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  scene=0;
  heif_context=heif_context_alloc();
  heif_image=(struct heif_image*) NULL;
  heif_encoder=(struct heif_encoder*) NULL;
  do
  {
    const Quantum
      *p;

#if LIBHEIF_NUMERIC_VERSION >= 0x01040000
    const StringInfo
      *profile;
#endif

    int
      stride_y,
      stride_cb,
      stride_cr;

    struct heif_error
      error;

    struct heif_writer
      writer;

    uint8_t
      *p_y,
      *p_cb,
      *p_cr;

    /*
      Transform colorspace to YCbCr.
    */
    if (image->colorspace != YCbCrColorspace)
      status=TransformImageColorspace(image,YCbCrColorspace,exception);
    if (status == MagickFalse)
      break;
    /*
      Initialize HEIF encoder context.
    */
    error=heif_image_create((int) image->columns,(int) image->rows,
      heif_colorspace_YCbCr,heif_chroma_420,&heif_image);
    status=IsHeifSuccess(&error,image,exception);
    if (status == MagickFalse)
      break;
#if LIBHEIF_NUMERIC_VERSION >= 0x01040000
    profile=GetImageProfile(image,"icc");
    if (profile != (StringInfo *) NULL)
      (void) heif_image_set_raw_color_profile(heif_image,"prof",
        GetStringInfoDatum(profile),GetStringInfoLength(profile));
#endif
    error=heif_image_add_plane(heif_image,heif_channel_Y,(int) image->columns,
      (int) image->rows,8);
    status=IsHeifSuccess(&error,image,exception);
    if (status == MagickFalse)
      break;
    error=heif_image_add_plane(heif_image,heif_channel_Cb,
      ((int) image->columns+1)/2,((int) image->rows+1)/2,8);
    status=IsHeifSuccess(&error,image,exception);
    if (status == MagickFalse)
      break;
    error=heif_image_add_plane(heif_image,heif_channel_Cr,
      ((int) image->columns+1)/2,((int) image->rows+1)/2,8);
    status=IsHeifSuccess(&error,image,exception);
    if (status == MagickFalse)
      break;
    p_y=heif_image_get_plane(heif_image,heif_channel_Y,&stride_y);
    p_cb=heif_image_get_plane(heif_image,heif_channel_Cb,&stride_cb);
    p_cr=heif_image_get_plane(heif_image,heif_channel_Cr,&stride_cr);
    /*
      Copy image to heif_image
    */
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      register ssize_t
        x;

      p=GetVirtualPixels(image,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        {
          status=MagickFalse;
          break;
        }
      if ((y & 0x01) == 0)
        for (x=0; x < (ssize_t) image->columns; x+=2)
        {
          p_y[y*stride_y+x]=ScaleQuantumToChar(GetPixelRed(image,p));
          p_cb[y/2*stride_cb+x/2]=ScaleQuantumToChar(GetPixelGreen(image,p));
          p_cr[y/2*stride_cr+x/2]=ScaleQuantumToChar(GetPixelBlue(image,p));
          p+=GetPixelChannels(image);
          if ((x+1) < (ssize_t) image->columns)
            {
              p_y[y*stride_y+x+1]=ScaleQuantumToChar(GetPixelRed(image,p));
              p+=GetPixelChannels(image);
            }
        }
      else
        for (x=0; x < (ssize_t) image->columns; x++)
        {
          p_y[y*stride_y+x]=ScaleQuantumToChar(GetPixelRed(image,p));
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
      break;
    /*
      Code and actually write the HEIC image
    */
    error=heif_context_get_encoder_for_format(heif_context,
      heif_compression_HEVC,&heif_encoder);
    status=IsHeifSuccess(&error,image,exception);
    if (status == MagickFalse)
      break;
    if (image_info->quality != UndefinedCompressionQuality)
      {
        error=heif_encoder_set_lossy_quality(heif_encoder,(int)
          image_info->quality);
        status=IsHeifSuccess(&error,image,exception);
        if (status == MagickFalse)
          break;
      }
    error=heif_context_encode_image(heif_context,heif_image,heif_encoder,
      (const struct heif_encoding_options *) NULL,
      (struct heif_image_handle **) NULL);
    status=IsHeifSuccess(&error,image,exception);
    if (status == MagickFalse)
      break;
    writer.writer_api_version=1;
    writer.write=heif_write_func;
#if LIBHEIF_NUMERIC_VERSION >= 0x01030000
    if (image->profiles != (void *) NULL)
      WriteProfile(heif_context, image, exception);
#endif
    error=heif_context_write(heif_context,&writer,image);
    status=IsHeifSuccess(&error,image,exception);
    if (status == MagickFalse)
      break;
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
    heif_encoder_release(heif_encoder);
    heif_encoder=(struct heif_encoder*) NULL;
    heif_image_release(heif_image);
    heif_image=(struct heif_image*) NULL;
    scene++;
  } while (image_info->adjoin != MagickFalse);
  if (heif_encoder != (struct heif_encoder*) NULL)
    heif_encoder_release(heif_encoder);
  if (heif_image != (struct heif_image*) NULL)
    heif_image_release(heif_image);
  heif_context_free(heif_context);
  (void) CloseBlob(image);
  return(status);
}