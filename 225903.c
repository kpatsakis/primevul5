MagickExport Image *SteganoImage(const Image *image,const Image *watermark,
  ExceptionInfo *exception)
{
#define GetBit(alpha,i) ((((size_t) (alpha) >> (size_t) (i)) & 0x01) != 0)
#define SetBit(alpha,i,set) (alpha)=(Quantum) ((set) != 0 ? (size_t) (alpha) \
  | (one << (size_t) (i)) : (size_t) (alpha) & ~(one << (size_t) (i)))
#define SteganoImageTag  "Stegano/Image"

  CacheView
    *stegano_view,
    *watermark_view;

  Image
    *stegano_image;

  int
    c;

  MagickBooleanType
    status;

  PixelPacket
    pixel;

  PixelPacket
    *q;

  ssize_t
    x;

  size_t
    depth,
    one;

  ssize_t
    i,
    j,
    k,
    y;

  /*
    Initialize steganographic image attributes.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(watermark != (const Image *) NULL);
  assert(watermark->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  one=1UL;
  stegano_image=CloneImage(image,0,0,MagickTrue,exception);
  if (stegano_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(stegano_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&stegano_image->exception);
      stegano_image=DestroyImage(stegano_image);
      return((Image *) NULL);
    }
  stegano_image->depth=MAGICKCORE_QUANTUM_DEPTH;
  /*
    Hide watermark in low-order bits of image.
  */
  c=0;
  i=0;
  j=0;
  depth=stegano_image->depth;
  k=image->offset;
  status=MagickTrue;
  watermark_view=AcquireVirtualCacheView(watermark,exception);
  stegano_view=AcquireAuthenticCacheView(stegano_image,exception);
  for (i=(ssize_t) depth-1; (i >= 0) && (j < (ssize_t) depth); i--)
  {
    for (y=0; (y < (ssize_t) watermark->rows) && (j < (ssize_t) depth); y++)
    {
      for (x=0; (x < (ssize_t) watermark->columns) && (j < (ssize_t) depth); x++)
      {
        (void) GetOneCacheViewVirtualPixel(watermark_view,x,y,&pixel,exception);
        if ((k/(ssize_t) stegano_image->columns) >= (ssize_t) stegano_image->rows)
          break;
        q=GetCacheViewAuthenticPixels(stegano_view,k % (ssize_t)
          stegano_image->columns,k/(ssize_t) stegano_image->columns,1,1,
          exception);
        if (q == (PixelPacket *) NULL)
          break;
        switch (c)
        {
          case 0:
          {
            SetBit(GetPixelRed(q),j,GetBit(ClampToQuantum(GetPixelIntensity(
              image,&pixel)),i));
            break;
          }
          case 1:
          {
            SetBit(GetPixelGreen(q),j,GetBit(ClampToQuantum(GetPixelIntensity(
              image,&pixel)),i));
            break;
          }
          case 2:
          {
            SetBit(GetPixelBlue(q),j,GetBit(ClampToQuantum(GetPixelIntensity(
              image,&pixel)),i));
            break;
          }
        }
        if (SyncCacheViewAuthenticPixels(stegano_view,exception) == MagickFalse)
          break;
        c++;
        if (c == 3)
          c=0;
        k++;
        if (k == (ssize_t) (stegano_image->columns*stegano_image->columns))
          k=0;
        if (k == image->offset)
          j++;
      }
    }
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(image,SteganoImageTag,(MagickOffsetType)
          (depth-i),depth);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  stegano_view=DestroyCacheView(stegano_view);
  watermark_view=DestroyCacheView(watermark_view);
  if (stegano_image->storage_class == PseudoClass)
    (void) SyncImage(stegano_image);
  if (status == MagickFalse)
    stegano_image=DestroyImage(stegano_image);
  return(stegano_image);
}