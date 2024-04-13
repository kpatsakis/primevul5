static struct heif_error heif_write_func(struct heif_context *context,
  const void* data,size_t size,void* userdata)
{
  Image
    *image;

  struct heif_error
    error_ok;

  (void) context;
  image=(Image*) userdata;
  (void) WriteBlob(image,size,(const unsigned char *) data);
  error_ok.code=heif_error_Ok;
  error_ok.subcode=heif_suberror_Unspecified;
  error_ok.message="ok";
  return(error_ok);
}