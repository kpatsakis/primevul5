static MagickBooleanType IsHeifSuccess(struct heif_error *error, Image *image)
{
  if (error->code == 0)
    return(MagickTrue);
  ThrowBinaryImageException(CorruptImageError,error->message,image->filename);
}