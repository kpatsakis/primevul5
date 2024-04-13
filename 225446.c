MagickExport Image *SparseColorImage(const Image *image,
  const SparseColorMethod method,const size_t number_arguments,
  const double *arguments,ExceptionInfo *exception)
{
#define SparseColorTag  "Distort/SparseColor"

  SparseColorMethod
    sparse_method;

  double
    *coeff;

  Image
    *sparse_image;

  size_t
    number_colors;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);

  /* Determine number of color values needed per control point */
  number_colors=0;
  if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
    number_colors++;
  if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
    number_colors++;
  if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
    number_colors++;
  if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
      (image->colorspace == CMYKColorspace))
    number_colors++;
  if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
      (image->alpha_trait != UndefinedPixelTrait))
    number_colors++;

  /*
    Convert input arguments into mapping coefficients, this this case
    we are mapping (distorting) colors, rather than coordinates.
  */
  { DistortMethod
      distort_method;

    distort_method=(DistortMethod) method;
    if ( distort_method >= SentinelDistortion )
      distort_method = ShepardsDistortion; /* Pretend to be Shepards */
    coeff = GenerateCoefficients(image, &distort_method, number_arguments,
                arguments, number_colors, exception);
    if ( coeff == (double *) NULL )
      return((Image *) NULL);
    /*
      Note some Distort Methods may fall back to other simpler methods,
      Currently the only fallback of concern is Bilinear to Affine
      (Barycentric), which is alaso sparse_colr method.  This also ensures
      correct two and one color Barycentric handling.
    */
    sparse_method = (SparseColorMethod) distort_method;
    if ( distort_method == ShepardsDistortion )
      sparse_method = method;   /* return non-distort methods to normal */
    if ( sparse_method == InverseColorInterpolate )
      coeff[0]=0.5;            /* sqrt() the squared distance for inverse */
  }

  /* Verbose output */
  if (IsStringTrue(GetImageArtifact(image,"verbose")) != MagickFalse) {

    switch (sparse_method) {
      case BarycentricColorInterpolate:
      {
        ssize_t x=0;
        (void) FormatLocaleFile(stderr, "Barycentric Sparse Color:\n");
        if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
          (void) FormatLocaleFile(stderr, "  -channel R -fx '%+lf*i %+lf*j %+lf' \\\n",
              coeff[x], coeff[x+1], coeff[x+2]),x+=3;
        if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
          (void) FormatLocaleFile(stderr, "  -channel G -fx '%+lf*i %+lf*j %+lf' \\\n",
              coeff[x], coeff[x+1], coeff[x+2]),x+=3;
        if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
          (void) FormatLocaleFile(stderr, "  -channel B -fx '%+lf*i %+lf*j %+lf' \\\n",
              coeff[x], coeff[x+1], coeff[x+2]),x+=3;
        if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
            (image->colorspace == CMYKColorspace))
          (void) FormatLocaleFile(stderr, "  -channel K -fx '%+lf*i %+lf*j %+lf' \\\n",
              coeff[x], coeff[x+1], coeff[x+2]),x+=3;
        if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
            (image->alpha_trait != UndefinedPixelTrait))
          (void) FormatLocaleFile(stderr, "  -channel A -fx '%+lf*i %+lf*j %+lf' \\\n",
              coeff[x], coeff[x+1], coeff[x+2]),x+=3;
        break;
      }
      case BilinearColorInterpolate:
      {
        ssize_t x=0;
        (void) FormatLocaleFile(stderr, "Bilinear Sparse Color\n");
        if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
          (void) FormatLocaleFile(stderr, "   -channel R -fx '%+lf*i %+lf*j %+lf*i*j %+lf;\n",
              coeff[ x ], coeff[x+1],
              coeff[x+2], coeff[x+3]),x+=4;
        if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
          (void) FormatLocaleFile(stderr, "   -channel G -fx '%+lf*i %+lf*j %+lf*i*j %+lf;\n",
              coeff[ x ], coeff[x+1],
              coeff[x+2], coeff[x+3]),x+=4;
        if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
          (void) FormatLocaleFile(stderr, "   -channel B -fx '%+lf*i %+lf*j %+lf*i*j %+lf;\n",
              coeff[ x ], coeff[x+1],
              coeff[x+2], coeff[x+3]),x+=4;
        if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
            (image->colorspace == CMYKColorspace))
          (void) FormatLocaleFile(stderr, "   -channel K -fx '%+lf*i %+lf*j %+lf*i*j %+lf;\n",
              coeff[ x ], coeff[x+1],
              coeff[x+2], coeff[x+3]),x+=4;
        if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
            (image->alpha_trait != UndefinedPixelTrait))
          (void) FormatLocaleFile(stderr, "   -channel A -fx '%+lf*i %+lf*j %+lf*i*j %+lf;\n",
              coeff[ x ], coeff[x+1],
              coeff[x+2], coeff[x+3]),x+=4;
        break;
      }
      default:
        /* sparse color method is too complex for FX emulation */
        break;
    }
  }

  /* Generate new image for generated interpolated gradient.
   * ASIDE: Actually we could have just replaced the colors of the original
   * image, but IM Core policy, is if storage class could change then clone
   * the image.
   */

  sparse_image=CloneImage(image,0,0,MagickTrue,exception);
  if (sparse_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(sparse_image,DirectClass,exception) == MagickFalse)
    { /* if image is ColorMapped - change it to DirectClass */
      sparse_image=DestroyImage(sparse_image);
      return((Image *) NULL);
    }
  { /* ----- MAIN CODE ----- */
    CacheView
      *sparse_view;

    MagickBooleanType
      status;

    MagickOffsetType
      progress;

    ssize_t
      j;

    status=MagickTrue;
    progress=0;
    sparse_view=AcquireAuthenticCacheView(sparse_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
    #pragma omp parallel for schedule(static) shared(progress,status) \
      magick_number_threads(image,sparse_image,sparse_image->rows,1)
#endif
    for (j=0; j < (ssize_t) sparse_image->rows; j++)
    {
      MagickBooleanType
        sync;

      PixelInfo
        pixel;    /* pixel to assign to distorted image */

      ssize_t
        i;

      Quantum
        *magick_restrict q;

      q=GetCacheViewAuthenticPixels(sparse_view,0,j,sparse_image->columns,
        1,exception);
      if (q == (Quantum *) NULL)
        {
          status=MagickFalse;
          continue;
        }
      GetPixelInfo(sparse_image,&pixel);
      for (i=0; i < (ssize_t) image->columns; i++)
      {
        GetPixelInfoPixel(image,q,&pixel);
        switch (sparse_method)
        {
          case BarycentricColorInterpolate:
          {
            ssize_t x=0;
            if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
              pixel.red     = coeff[x]*i +coeff[x+1]*j
                              +coeff[x+2], x+=3;
            if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
              pixel.green   = coeff[x]*i +coeff[x+1]*j
                              +coeff[x+2], x+=3;
            if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
              pixel.blue    = coeff[x]*i +coeff[x+1]*j
                              +coeff[x+2], x+=3;
            if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
                (image->colorspace == CMYKColorspace))
              pixel.black   = coeff[x]*i +coeff[x+1]*j
                              +coeff[x+2], x+=3;
            if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
                (image->alpha_trait != UndefinedPixelTrait))
              pixel.alpha = coeff[x]*i +coeff[x+1]*j
                              +coeff[x+2], x+=3;
            break;
          }
          case BilinearColorInterpolate:
          {
            ssize_t x=0;
            if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
              pixel.red     = coeff[x]*i     + coeff[x+1]*j +
                              coeff[x+2]*i*j + coeff[x+3], x+=4;
            if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
              pixel.green   = coeff[x]*i     + coeff[x+1]*j +
                              coeff[x+2]*i*j + coeff[x+3], x+=4;
            if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
              pixel.blue    = coeff[x]*i     + coeff[x+1]*j +
                              coeff[x+2]*i*j + coeff[x+3], x+=4;
            if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
                (image->colorspace == CMYKColorspace))
              pixel.black   = coeff[x]*i     + coeff[x+1]*j +
                              coeff[x+2]*i*j + coeff[x+3], x+=4;
            if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
                (image->alpha_trait != UndefinedPixelTrait))
              pixel.alpha = coeff[x]*i     + coeff[x+1]*j +
                              coeff[x+2]*i*j + coeff[x+3], x+=4;
            break;
          }
          case InverseColorInterpolate:
          case ShepardsColorInterpolate:
          { /* Inverse (Squared) Distance weights average (IDW) */
            size_t
              k;
            double
              denominator;

            if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
              pixel.red=0.0;
            if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
              pixel.green=0.0;
            if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
              pixel.blue=0.0;
            if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
                (image->colorspace == CMYKColorspace))
              pixel.black=0.0;
            if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
                (image->alpha_trait != UndefinedPixelTrait))
              pixel.alpha=0.0;
            denominator = 0.0;
            for(k=0; k<number_arguments; k+=2+number_colors) {
              ssize_t x=(ssize_t) k+2;
              double weight =
                  ((double)i-arguments[ k ])*((double)i-arguments[ k ])
                + ((double)j-arguments[k+1])*((double)j-arguments[k+1]);
              weight = pow(weight,coeff[0]); /* inverse of power factor */
              weight = ( weight < 1.0 ) ? 1.0 : 1.0/weight;
              if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
                pixel.red     += arguments[x++]*weight;
              if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
                pixel.green   += arguments[x++]*weight;
              if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
                pixel.blue    += arguments[x++]*weight;
              if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
                  (image->colorspace == CMYKColorspace))
                pixel.black   += arguments[x++]*weight;
              if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
                  (image->alpha_trait != UndefinedPixelTrait))
                pixel.alpha += arguments[x++]*weight;
              denominator += weight;
            }
            if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
              pixel.red/=denominator;
            if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
              pixel.green/=denominator;
            if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
              pixel.blue/=denominator;
            if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
                (image->colorspace == CMYKColorspace))
              pixel.black/=denominator;
            if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
                (image->alpha_trait != UndefinedPixelTrait))
              pixel.alpha/=denominator;
            break;
          }
          case ManhattanColorInterpolate:
          {
            size_t
              k;

            double
              minimum = MagickMaximumValue;

            /*
              Just use the closest control point you can find!
            */
            for(k=0; k<number_arguments; k+=2+number_colors) {
              double distance =
                  fabs((double)i-arguments[ k ])
                + fabs((double)j-arguments[k+1]);
              if ( distance < minimum ) {
                ssize_t x=(ssize_t) k+2;
                if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
                  pixel.red=arguments[x++];
                if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
                  pixel.green=arguments[x++];
                if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
                  pixel.blue=arguments[x++];
                if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
                    (image->colorspace == CMYKColorspace))
                  pixel.black=arguments[x++];
                if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
                    (image->alpha_trait != UndefinedPixelTrait))
                  pixel.alpha=arguments[x++];
                minimum = distance;
              }
            }
            break;
          }
          case VoronoiColorInterpolate:
          default:
          {
            size_t
              k;

            double
              minimum = MagickMaximumValue;

            /*
              Just use the closest control point you can find!
            */
            for (k=0; k<number_arguments; k+=2+number_colors) {
              double distance =
                  ((double)i-arguments[ k ])*((double)i-arguments[ k ])
                + ((double)j-arguments[k+1])*((double)j-arguments[k+1]);
              if ( distance < minimum ) {
                ssize_t x=(ssize_t) k+2;
                if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
                  pixel.red=arguments[x++];
                if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
                  pixel.green=arguments[x++];
                if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
                  pixel.blue=arguments[x++];
                if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
                    (image->colorspace == CMYKColorspace))
                  pixel.black=arguments[x++];
                if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
                    (image->alpha_trait != UndefinedPixelTrait))
                  pixel.alpha=arguments[x++];
                minimum = distance;
              }
            }
            break;
          }
        }
        /* set the color directly back into the source image */
        if ((GetPixelRedTraits(image) & UpdatePixelTrait) != 0)
          pixel.red=(MagickRealType) ClampPixel(QuantumRange*pixel.red);
        if ((GetPixelGreenTraits(image) & UpdatePixelTrait) != 0)
          pixel.green=(MagickRealType) ClampPixel(QuantumRange*pixel.green);
        if ((GetPixelBlueTraits(image) & UpdatePixelTrait) != 0)
          pixel.blue=(MagickRealType) ClampPixel(QuantumRange*pixel.blue);
        if (((GetPixelBlackTraits(image) & UpdatePixelTrait) != 0) &&
            (image->colorspace == CMYKColorspace))
          pixel.black=(MagickRealType) ClampPixel(QuantumRange*pixel.black);
        if (((GetPixelAlphaTraits(image) & UpdatePixelTrait) != 0) &&
            (image->alpha_trait != UndefinedPixelTrait))
          pixel.alpha=(MagickRealType) ClampPixel(QuantumRange*pixel.alpha);
        SetPixelViaPixelInfo(sparse_image,&pixel,q);
        q+=GetPixelChannels(sparse_image);
      }
      sync=SyncCacheViewAuthenticPixels(sparse_view,exception);
      if (sync == MagickFalse)
        status=MagickFalse;
      if (image->progress_monitor != (MagickProgressMonitor) NULL)
        {
          MagickBooleanType
            proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
          #pragma omp atomic
#endif
          progress++;
          proceed=SetImageProgress(image,SparseColorTag,progress,image->rows);
          if (proceed == MagickFalse)
            status=MagickFalse;
        }
    }
    sparse_view=DestroyCacheView(sparse_view);
    if (status == MagickFalse)
      sparse_image=DestroyImage(sparse_image);
  }
  coeff = (double *) RelinquishMagickMemory(coeff);
  return(sparse_image);
}