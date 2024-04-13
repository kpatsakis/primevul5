static MagickBooleanType ReadHEICColorProfile(Image *image,
  struct heif_image_handle *image_handle,ExceptionInfo *exception)
{
  size_t
    length;

#if LIBHEIF_NUMERIC_VERSION >= 0x01040000
  length=heif_image_handle_get_raw_color_profile_size(image_handle);
  if (length > 0)
    {
      unsigned char
        *color_buffer;

      /*
        Read color profile.
      */
      if ((MagickSizeType) length > GetBlobSize(image))
        ThrowBinaryException(CorruptImageError,"InsufficientImageDataInFile",
          image->filename);
      color_buffer=(unsigned char *) AcquireMagickMemory(length);
      if (color_buffer != (unsigned char *) NULL)
        {
          struct heif_error
            error;

          error=heif_image_handle_get_raw_color_profile(image_handle,
            color_buffer);
          if (error.code == 0)
            {
              StringInfo
                *profile;

              profile=BlobToStringInfo(color_buffer,length);
              if (profile != (StringInfo*) NULL)
                {
                  (void) SetImageProfile(image,"icc",profile,exception);
                  profile=DestroyStringInfo(profile);
                }
            }
        }
      color_buffer=(unsigned char *) RelinquishMagickMemory(color_buffer);
    }
#endif
  return(MagickTrue);
}