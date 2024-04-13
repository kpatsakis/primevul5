MagickExport Image *SketchImage(const Image *image,const double radius,
  const double sigma,const double angle,ExceptionInfo *exception)
{
  CacheView
    *random_view;

  Image
    *blend_image,
    *blur_image,
    *dodge_image,
    *random_image,
    *sketch_image;

  MagickBooleanType
    status;

  MagickPixelPacket
    zero;

  RandomInfo
    **magick_restrict random_info;

  ssize_t
    y;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
  unsigned long
    key;
#endif

  /*
    Sketch image.
  */
  random_image=CloneImage(image,image->columns << 1,image->rows << 1,
    MagickTrue,exception);
  if (random_image == (Image *) NULL)
    return((Image *) NULL);
  status=MagickTrue;
  GetMagickPixelPacket(random_image,&zero);
  random_info=AcquireRandomInfoThreadSet();
  random_view=AcquireAuthenticCacheView(random_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  key=GetRandomSecretKey(random_info[0]);
#pragma omp parallel for schedule(static) shared(status) \
magick_number_threads(random_image,random_image,random_image->rows,key == ~0UL)
#endif
  for (y=0; y < (ssize_t) random_image->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    MagickPixelPacket
      pixel;

    IndexPacket
      *magick_restrict indexes;

    ssize_t
      x;

    PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    q=QueueCacheViewAuthenticPixels(random_view,0,y,random_image->columns,1,
      exception);
    if (q == (PixelPacket *) NULL)
    {
      status=MagickFalse;
      continue;
    }
    indexes=GetCacheViewAuthenticIndexQueue(random_view);
    pixel=zero;
    for (x=0; x < (ssize_t) random_image->columns; x++)
    {
      pixel.red=(MagickRealType) (QuantumRange*
        GetPseudoRandomValue(random_info[id]));
      pixel.green=pixel.red;
      pixel.blue=pixel.red;
      if (image->colorspace == CMYKColorspace)
        pixel.index=pixel.red;
      SetPixelPacket(random_image,&pixel,q,indexes+x);
      q++;
    }
    if (SyncCacheViewAuthenticPixels(random_view,exception) == MagickFalse)
      status=MagickFalse;
  }
  random_info=DestroyRandomInfoThreadSet(random_info);
  if (status == MagickFalse)
    {
      random_view=DestroyCacheView(random_view);
      random_image=DestroyImage(random_image);
      return(random_image);
    }
  random_view=DestroyCacheView(random_view);

  blur_image=MotionBlurImage(random_image,radius,sigma,angle,exception);
  random_image=DestroyImage(random_image);
  if (blur_image == (Image *) NULL)
    return((Image *) NULL);
  dodge_image=EdgeImage(blur_image,radius,exception);
  blur_image=DestroyImage(blur_image);
  if (dodge_image == (Image *) NULL)
    return((Image *) NULL);
  status=ClampImage(dodge_image);
  if (status != MagickFalse)
    status=NormalizeImage(dodge_image);
  if (status != MagickFalse)
    status=NegateImage(dodge_image,MagickFalse);
  if (status != MagickFalse)
    status=TransformImage(&dodge_image,(char *) NULL,"50%");
  sketch_image=CloneImage(image,0,0,MagickTrue,exception);
  if (sketch_image == (Image *) NULL)
    {
      dodge_image=DestroyImage(dodge_image);
      return((Image *) NULL);
    }
  (void) CompositeImage(sketch_image,ColorDodgeCompositeOp,dodge_image,0,0);
  dodge_image=DestroyImage(dodge_image);
  blend_image=CloneImage(image,0,0,MagickTrue,exception);
  if (blend_image == (Image *) NULL)
    {
      sketch_image=DestroyImage(sketch_image);
      return((Image *) NULL);
    }
  (void) SetImageArtifact(blend_image,"compose:args","20x80");
  (void) CompositeImage(sketch_image,BlendCompositeOp,blend_image,0,0);
  blend_image=DestroyImage(blend_image);
  return(sketch_image);
}