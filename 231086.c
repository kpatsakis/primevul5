MagickExport MagickBooleanType ClipImagePath(Image *image,const char *pathname,
  const MagickBooleanType inside)
{
#define ClipImagePathTag  "ClipPath/Image"

  char
    *property;

  const char
    *value;

  Image
    *clip_mask;

  ImageInfo
    *image_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(pathname != NULL);
  property=AcquireString(pathname);
  (void) FormatLocaleString(property,MaxTextExtent,"8BIM:1999,2998:%s",
    pathname);
  value=GetImageProperty(image,property);
  property=DestroyString(property);
  if (value == (const char *) NULL)
    {
      ThrowFileException(&image->exception,OptionError,"NoClipPathDefined",
        image->filename);
      return(MagickFalse);
    }
  image_info=AcquireImageInfo();
  (void) CopyMagickString(image_info->filename,image->filename,MaxTextExtent);
  (void) ConcatenateMagickString(image_info->filename,pathname,MaxTextExtent);
  clip_mask=BlobToImage(image_info,value,strlen(value),&image->exception);
  image_info=DestroyImageInfo(image_info);
  if (clip_mask == (Image *) NULL)
    return(MagickFalse);
  if (clip_mask->storage_class == PseudoClass)
    {
      (void) SyncImage(clip_mask);
      if (SetImageStorageClass(clip_mask,DirectClass) == MagickFalse)
        return(MagickFalse);
    }
  if (inside == MagickFalse)
    (void) NegateImage(clip_mask,MagickFalse);
  (void) FormatLocaleString(clip_mask->magick_filename,MaxTextExtent,
    "8BIM:1999,2998:%s\nPS",pathname);
  (void) SetImageClipMask(image,clip_mask);
  clip_mask=DestroyImage(clip_mask);
  return(MagickTrue);
}