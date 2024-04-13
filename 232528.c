static MagickBooleanType ReadHEICExifProfile(Image *image,
  struct heif_image_handle *image_handle,ExceptionInfo *exception)
{
  heif_item_id
    exif_id;

  int
    count;

  count=heif_image_handle_get_list_of_metadata_block_IDs(image_handle,"Exif",
    &exif_id,1);
  if (count > 0)
    {
      size_t
        exif_size;

      unsigned char
        *exif_buffer;

      /*
        Read Exif profile.
      */
      exif_size=heif_image_handle_get_metadata_size(image_handle,exif_id);
      if ((MagickSizeType) exif_size > GetBlobSize(image))
        ThrowBinaryException(CorruptImageError,"InsufficientImageDataInFile",
          image->filename);
      exif_buffer=(unsigned char *) AcquireMagickMemory(exif_size);
      if (exif_buffer != (unsigned char *) NULL)
        {
          struct heif_error
            error;

          error=heif_image_handle_get_metadata(image_handle,
            exif_id,exif_buffer);
          if (error.code == 0)
            {
              StringInfo
                *profile;

              /*
                The first 4 byte should be skipped since they indicate the
                offset to the start of the TIFF header of the Exif data.
              */
              profile=(StringInfo*) NULL;
              if (exif_size > 8)
                profile=BlobToStringInfo(exif_buffer+4,(size_t) exif_size-4);
              if (profile != (StringInfo*) NULL)
                {
                  (void) SetImageProfile(image,"exif",profile,exception);
                  profile=DestroyStringInfo(profile);
                }
            }
        }
      exif_buffer=(unsigned char *) RelinquishMagickMemory(exif_buffer);
  }
  return(MagickTrue);
}