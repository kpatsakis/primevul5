MagickExport MagickBooleanType SyncImage(Image *image)
{
  CacheView
    *image_view;

  ExceptionInfo
    *exception;

  MagickBooleanType
    range_exception,
    status,
    taint;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image->signature == MagickCoreSignature);
  if (image->ping != MagickFalse)
    return(MagickTrue);
  if (image->storage_class != PseudoClass)
    return(MagickFalse);
  assert(image->colormap != (PixelPacket *) NULL);
  range_exception=MagickFalse;
  status=MagickTrue;
  taint=image->taint;
  exception=(&image->exception);
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(range_exception,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    IndexPacket
      index;

    register IndexPacket
      *magick_restrict indexes;

    register PixelPacket
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (PixelPacket *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewAuthenticIndexQueue(image_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      index=PushColormapIndex(image,(size_t) GetPixelIndex(indexes+x),
        &range_exception);
      if (image->matte == MagickFalse)
        SetPixelRgb(q,image->colormap+(ssize_t) index)
      else
        SetPixelRGBO(q,image->colormap+(ssize_t) index);
      q++;
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  image_view=DestroyCacheView(image_view);
  image->taint=taint;
  if ((image->ping == MagickFalse) && (range_exception != MagickFalse))
    (void) ThrowMagickException(&image->exception,GetMagickModule(),
      CorruptImageWarning,"InvalidColormapIndex","`%s'",image->filename);
  return(status);
}