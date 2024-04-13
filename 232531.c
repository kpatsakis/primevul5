static Image *ReadHEICImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  const StringInfo
    *profile;

  heif_item_id
    *image_ids,
    primary_image_id;

  Image
    *image;

  MagickBooleanType
    status;

  size_t
    count,
    length;

  struct heif_context
    *heif_context;

  struct heif_error
    error;

  void
    *file_data;

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
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  if (GetBlobSize(image) > (MagickSizeType) SSIZE_MAX)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  length=(size_t) GetBlobSize(image);
  file_data=AcquireMagickMemory(length);
  if (file_data == (void *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  if (ReadBlob(image,length,file_data) != (ssize_t) length)
    {
      file_data=RelinquishMagickMemory(file_data);
      ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
    }
  /*
    Decode HEIF file
  */
  heif_context=heif_context_alloc();
  error=heif_context_read_from_memory_without_copy(heif_context,file_data,
    length,NULL);
  if (IsHeifSuccess(&error,image,exception) == MagickFalse)
    {
      heif_context_free(heif_context);
      file_data=RelinquishMagickMemory(file_data);
      return(DestroyImageList(image));
    }
  error=heif_context_get_primary_image_ID(heif_context,&primary_image_id);
  if (IsHeifSuccess(&error,image,exception) == MagickFalse)
    {
      heif_context_free(heif_context);
      file_data=RelinquishMagickMemory(file_data);
      return(DestroyImageList(image));
    }
  status=ReadHEICImageByID(image_info,image,heif_context,primary_image_id,
    exception);
  image_ids=(heif_item_id *) NULL;
  count=(size_t) heif_context_get_number_of_top_level_images(heif_context);
  if ((status != MagickFalse) && (count > 1))
    {
      register size_t
        i;

      image_ids=(heif_item_id *) AcquireQuantumMemory((size_t) count,
        sizeof(*image_ids));
      if (image_ids == (heif_item_id *) NULL)
        {
          heif_context_free(heif_context);
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        }
      (void) heif_context_get_list_of_top_level_image_IDs(heif_context,
        image_ids,(int) count);
      for (i=0; i < count; i++)
      {
        if (image_ids[i] == primary_image_id)
          continue;
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image,exception);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            status=MagickFalse;
            break;
          }
        image=SyncNextImageInList(image);
        status=ReadHEICImageByID(image_info,image,heif_context,image_ids[i],
          exception);
        if (status == MagickFalse)
          break;
        if (image_info->number_scenes != 0)
          if (image->scene >= (image_info->scene+image_info->number_scenes-1))
            break;
      }
    }
  if (image_ids != (heif_item_id *) NULL)
    (void) RelinquishMagickMemory(image_ids);
  heif_context_free(heif_context);
  file_data=RelinquishMagickMemory(file_data);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  /*
    Change image colorspace if it contains a color profile.
  */
  image=GetFirstImageInList(image);
  profile=GetImageProfile(image,"icc");
  if (profile != (const StringInfo *) NULL)
    {
      Image
        *next;

      next=image;
      while (next != (Image *) NULL)
      {
        if (HEICSkipImage(image_info,next) != MagickFalse)
          {
            if (image_info->ping == MagickFalse)
              (void) TransformImageColorspace(next,sRGBColorspace,exception);
            else
              next->colorspace=sRGBColorspace;
          }
        next=GetNextImageInList(next);
      }
    }
  return(image);
}