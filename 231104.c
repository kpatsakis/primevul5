MagickExport Image *AppendImages(const Image *images,
  const MagickBooleanType stack,ExceptionInfo *exception)
{
#define AppendImageTag  "Append/Image"

  CacheView
    *append_view;

  Image
    *append_image;

  MagickBooleanType
    homogeneous_colorspace,
    matte,
    status;

  MagickOffsetType
    n;

  RectangleInfo
    geometry;

  register const Image
    *next;

  size_t
    depth,
    height,
    number_images,
    width;

  ssize_t
    x_offset,
    y,
    y_offset;

  /*
    Compute maximum area of appended area.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  matte=images->matte;
  number_images=1;
  width=images->columns;
  height=images->rows;
  depth=images->depth;
  homogeneous_colorspace=MagickTrue;
  next=GetNextImageInList(images);
  for ( ; next != (Image *) NULL; next=GetNextImageInList(next))
  {
    if (next->depth > depth)
      depth=next->depth;
    if (next->colorspace != images->colorspace)
      homogeneous_colorspace=MagickFalse;
    if (next->matte != MagickFalse)
      matte=MagickTrue;
    number_images++;
    if (stack != MagickFalse)
      {
        if (next->columns > width)
          width=next->columns;
        height+=next->rows;
        continue;
      }
    width+=next->columns;
    if (next->rows > height)
      height=next->rows;
  }
  /*
    Append images.
  */
  append_image=CloneImage(images,width,height,MagickTrue,exception);
  if (append_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(append_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&append_image->exception);
      append_image=DestroyImage(append_image);
      return((Image *) NULL);
    }
  if (homogeneous_colorspace == MagickFalse)
    (void) SetImageColorspace(append_image,sRGBColorspace);
  append_image->depth=depth;
  append_image->matte=matte;
  append_image->page=images->page;
  (void) SetImageBackgroundColor(append_image);
  status=MagickTrue;
  x_offset=0;
  y_offset=0;
  next=images;
  append_view=AcquireAuthenticCacheView(append_image,exception);
  for (n=0; n < (MagickOffsetType) number_images; n++)
  {
    CacheView
      *image_view;

    MagickBooleanType
      proceed;

    SetGeometry(append_image,&geometry);
    GravityAdjustGeometry(next->columns,next->rows,next->gravity,&geometry);
    if (stack != MagickFalse)
      x_offset-=geometry.x;
    else
      y_offset-=geometry.y;
    image_view=AcquireVirtualCacheView(next,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
    #pragma omp parallel for schedule(static) shared(status) \
      magick_number_threads(next,next,next->rows,1)
#endif
    for (y=0; y < (ssize_t) next->rows; y++)
    {
      MagickBooleanType
        sync;

      register const IndexPacket
        *magick_restrict indexes;

      register const PixelPacket
        *magick_restrict p;

      register IndexPacket
        *magick_restrict append_indexes;

      register PixelPacket
        *magick_restrict q;

      register ssize_t
        x;

      if (status == MagickFalse)
        continue;
      p=GetCacheViewVirtualPixels(image_view,0,y,next->columns,1,exception);
      q=QueueCacheViewAuthenticPixels(append_view,x_offset,y+y_offset,
        next->columns,1,exception);
      if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
        {
          status=MagickFalse;
          continue;
        }
      indexes=GetCacheViewVirtualIndexQueue(image_view);
      append_indexes=GetCacheViewAuthenticIndexQueue(append_view);
      for (x=0; x < (ssize_t) next->columns; x++)
      {
        SetPixelRed(q,GetPixelRed(p));
        SetPixelGreen(q,GetPixelGreen(p));
        SetPixelBlue(q,GetPixelBlue(p));
        SetPixelOpacity(q,OpaqueOpacity);
        if (next->matte != MagickFalse)
          SetPixelOpacity(q,GetPixelOpacity(p));
        if ((next->colorspace == CMYKColorspace) &&
            (append_image->colorspace == CMYKColorspace))
          SetPixelIndex(append_indexes+x,GetPixelIndex(indexes+x));
        p++;
        q++;
      }
      sync=SyncCacheViewAuthenticPixels(append_view,exception);
      if (sync == MagickFalse)
        status=MagickFalse;
    }
    image_view=DestroyCacheView(image_view);
    if (stack == MagickFalse)
      {
        x_offset+=(ssize_t) next->columns;
        y_offset=0;
      }
    else
      {
        x_offset=0;
        y_offset+=(ssize_t) next->rows;
      }
    proceed=SetImageProgress(append_image,AppendImageTag,n,number_images);
    if (proceed == MagickFalse)
      break;
    next=GetNextImageInList(next);
  }
  append_view=DestroyCacheView(append_view);
  if (status == MagickFalse)
    append_image=DestroyImage(append_image);
  return(append_image);
}