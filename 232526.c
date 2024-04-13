static void WriteProfile(struct heif_context *context,Image *image,
  ExceptionInfo *exception)
{
  const char
    *name;

  const StringInfo
    *profile;

  register ssize_t
    i;

  size_t
    length;

  struct heif_error
    error;

  struct heif_image_handle
    *image_handle;

  /*
    Get image handle.
  */
  image_handle=(struct heif_image_handle *) NULL;
  error=heif_context_get_primary_image_handle(context,&image_handle);
  if (error.code != 0)
    return;
  /*
    Save image profile as a APP marker.
  */
  ResetImageProfileIterator(image);
  for (name=GetNextImageProfile(image); name != (const char *) NULL; )
  {
    profile=GetImageProfile(image,name);
    length=GetStringInfoLength(profile);
    if (LocaleCompare(name,"EXIF") == 0)
      {
        length=GetStringInfoLength(profile);
        if (length > 65533L)
          {
            (void) ThrowMagickException(exception,GetMagickModule(),
              CoderWarning,"ExifProfileSizeExceedsLimit","`%s'",
              image->filename);
            length=65533L;
          }
          (void) heif_context_add_exif_metadata(context,image_handle,
            (void*) GetStringInfoDatum(profile),length);
      }
    if (LocaleCompare(name,"XMP") == 0)
      {
        StringInfo
          *xmp_profile;

        xmp_profile=StringToStringInfo(xmp_namespace);
        if (xmp_profile != (StringInfo *) NULL)
          {
            if (profile != (StringInfo *) NULL)
              ConcatenateStringInfo(xmp_profile,profile);
            GetStringInfoDatum(xmp_profile)[XmpNamespaceExtent]='\0';
            for (i=0; i < (ssize_t) GetStringInfoLength(xmp_profile); i+=65533L)
            {
              length=MagickMin(GetStringInfoLength(xmp_profile)-i,65533L);
              error=heif_context_add_XMP_metadata(context,image_handle,
                (void*) (GetStringInfoDatum(xmp_profile)+i),length);
              if (error.code != 0)
                break;
            }
            xmp_profile=DestroyStringInfo(xmp_profile);
          }
      }
    if (image->debug != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "%s profile: %.20g bytes",name,(double) GetStringInfoLength(profile));
    name=GetNextImageProfile(image);
  }
  heif_image_handle_release(image_handle);
}