MagickExport MagickBooleanType ResetImagePixels(Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  const void
    *pixels;

  MagickBooleanType
    status;

  MagickSizeType
    length;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  pixels=AcquirePixelCachePixels(image,&length,exception);
  if (pixels != (void *) NULL)
    {
      /*
        Reset in-core image pixels.
      */
      (void) memset((void *) pixels,0,(size_t) length);
      return(MagickTrue);
    }
  /*
    Reset image pixels.
  */
  status=MagickTrue;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register IndexPacket
      *magick_restrict indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      (void) memset(q,0,sizeof(PixelPacket));
      if ((image->storage_class == PseudoClass) ||
          (image->colorspace == CMYKColorspace))
        indexes[x]=0;
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  return(status);
}