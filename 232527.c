static MagickBooleanType IsHeifSuccess(struct heif_error *error,Image *image,
  ExceptionInfo *exception)
{
  if (error->code == 0)
    return(MagickTrue);
  ThrowBinaryException(CorruptImageError,error->message,image->filename);
}