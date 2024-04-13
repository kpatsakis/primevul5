MagickExport Image *ColorMatrixImage(const Image *image,
  const KernelInfo *color_matrix,ExceptionInfo *exception)
{
#define ColorMatrixImageTag  "ColorMatrix/Image"

  CacheView
    *color_view,
    *image_view;

  double
    ColorMatrix[6][6] =
    {
      { 1.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
      { 0.0, 1.0, 0.0, 0.0, 0.0, 0.0 },
      { 0.0, 0.0, 1.0, 0.0, 0.0, 0.0 },
      { 0.0, 0.0, 0.0, 1.0, 0.0, 0.0 },
      { 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 },
      { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 }
    };

  Image
    *color_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    i;

  ssize_t
    u,
    v,
    y;

  /*
    Create color matrix.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  i=0;
  for (v=0; v < (ssize_t) color_matrix->height; v++)
    for (u=0; u < (ssize_t) color_matrix->width; u++)
    {
      if ((v < 6) && (u < 6))
        ColorMatrix[v][u]=color_matrix->values[i];
      i++;
    }
  /*
    Initialize color image.
  */
  color_image=CloneImage(image,0,0,MagickTrue,exception);
  if (color_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(color_image,DirectClass) == MagickFalse)
    {
      InheritException(exception,&color_image->exception);
      color_image=DestroyImage(color_image);
      return((Image *) NULL);
    }
  if (image->debug != MagickFalse)
    {
      char
        format[MaxTextExtent],
        *message;

      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  ColorMatrix image with color matrix:");
      message=AcquireString("");
      for (v=0; v < 6; v++)
      {
        *message='\0';
        (void) FormatLocaleString(format,MaxTextExtent,"%.20g: ",(double) v);
        (void) ConcatenateString(&message,format);
        for (u=0; u < 6; u++)
        {
          (void) FormatLocaleString(format,MaxTextExtent,"%+f ",
            ColorMatrix[v][u]);
          (void) ConcatenateString(&message,format);
        }
        (void) LogMagickEvent(TransformEvent,GetMagickModule(),"%s",message);
      }
      message=DestroyString(message);
    }
  /*
    ColorMatrix image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  color_view=AcquireAuthenticCacheView(color_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,color_image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    MagickRealType
      pixel;

    const IndexPacket
      *magick_restrict indexes;

    const PixelPacket
      *magick_restrict p;

    ssize_t
      x;

    IndexPacket
      *magick_restrict color_indexes;

    PixelPacket
      *magick_restrict q;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=GetCacheViewAuthenticPixels(color_view,0,y,color_image->columns,1,
      exception);
    if ((p == (const PixelPacket *) NULL) || (q == (PixelPacket *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    indexes=GetCacheViewVirtualIndexQueue(image_view);
    color_indexes=GetCacheViewAuthenticIndexQueue(color_view);
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      ssize_t
        v;

      size_t
        height;

      height=color_matrix->height > 6 ? 6UL : color_matrix->height;
      for (v=0; v < (ssize_t) height; v++)
      {
        pixel=ColorMatrix[v][0]*GetPixelRed(p)+ColorMatrix[v][1]*
          GetPixelGreen(p)+ColorMatrix[v][2]*GetPixelBlue(p);
        if (image->matte != MagickFalse)
          pixel+=ColorMatrix[v][3]*(QuantumRange-GetPixelOpacity(p));
        if (image->colorspace == CMYKColorspace)
          pixel+=ColorMatrix[v][4]*GetPixelIndex(indexes+x);
        pixel+=QuantumRange*ColorMatrix[v][5];
        switch (v)
        {
          case 0: SetPixelRed(q,ClampToQuantum(pixel)); break;
          case 1: SetPixelGreen(q,ClampToQuantum(pixel)); break;
          case 2: SetPixelBlue(q,ClampToQuantum(pixel)); break;
          case 3:
          {
            if (image->matte != MagickFalse)
              SetPixelAlpha(q,ClampToQuantum(pixel));
            break;
          }
          case 4:
          {
            if (image->colorspace == CMYKColorspace)
              SetPixelIndex(color_indexes+x,ClampToQuantum(pixel));
            break;
          }
        }
      }
      p++;
      q++;
    }
    if (SyncCacheViewAuthenticPixels(color_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,ColorMatrixImageTag,progress,
          image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  color_view=DestroyCacheView(color_view);
  image_view=DestroyCacheView(image_view);
  if (status == MagickFalse)
    color_image=DestroyImage(color_image);
  return(color_image);
}