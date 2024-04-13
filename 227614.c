MagickExport MagickBooleanType ResamplePixelColor(
  ResampleFilter *resample_filter,const double u0,const double v0,
  PixelInfo *pixel,ExceptionInfo *exception)
{
  MagickBooleanType
    status;

  ssize_t u,v, v1, v2, uw, hit;
  double u1;
  double U,V,Q,DQ,DDQ;
  double divisor_c,divisor_m;
  double weight;
  const Quantum *pixels;
  assert(resample_filter != (ResampleFilter *) NULL);
  assert(resample_filter->signature == MagickCoreSignature);

  status=MagickTrue;
  /* GetPixelInfo(resample_filter->image,pixel); */
  if ( resample_filter->do_interpolate ) {
    status=InterpolatePixelInfo(resample_filter->image,resample_filter->view,
      resample_filter->interpolate,u0,v0,pixel,resample_filter->exception);
    return(status);
  }

#if DEBUG_ELLIPSE
  (void) FormatLocaleFile(stderr, "u0=%lf; v0=%lf;\n", u0, v0);
#endif

  /*
    Does resample area Miss the image Proper?
    If and that area a simple solid color - then simply return that color!
    This saves a lot of calculation when resampling outside the bounds of
    the source image.

    However it probably should be expanded to image bounds plus the filters
    scaled support size.
  */
  hit = 0;
  switch ( resample_filter->virtual_pixel ) {
    case BackgroundVirtualPixelMethod:
    case TransparentVirtualPixelMethod:
    case BlackVirtualPixelMethod:
    case GrayVirtualPixelMethod:
    case WhiteVirtualPixelMethod:
    case MaskVirtualPixelMethod:
      if ( resample_filter->limit_reached
           || u0 + resample_filter->Ulimit < 0.0
           || u0 - resample_filter->Ulimit > (double) resample_filter->image->columns-1.0
           || v0 + resample_filter->Vlimit < 0.0
           || v0 - resample_filter->Vlimit > (double) resample_filter->image->rows-1.0
           )
        hit++;
      break;

    case UndefinedVirtualPixelMethod:
    case EdgeVirtualPixelMethod:
      if (    ( u0 + resample_filter->Ulimit < 0.0 && v0 + resample_filter->Vlimit < 0.0 )
           || ( u0 + resample_filter->Ulimit < 0.0
                && v0 - resample_filter->Vlimit > (double) resample_filter->image->rows-1.0 )
           || ( u0 - resample_filter->Ulimit > (double) resample_filter->image->columns-1.0
                && v0 + resample_filter->Vlimit < 0.0 )
           || ( u0 - resample_filter->Ulimit > (double) resample_filter->image->columns-1.0
                && v0 - resample_filter->Vlimit > (double) resample_filter->image->rows-1.0 )
           )
        hit++;
      break;
    case HorizontalTileVirtualPixelMethod:
      if (    v0 + resample_filter->Vlimit < 0.0
           || v0 - resample_filter->Vlimit > (double) resample_filter->image->rows-1.0
           )
        hit++;  /* outside the horizontally tiled images. */
      break;
    case VerticalTileVirtualPixelMethod:
      if (    u0 + resample_filter->Ulimit < 0.0
           || u0 - resample_filter->Ulimit > (double) resample_filter->image->columns-1.0
           )
        hit++;  /* outside the vertically tiled images. */
      break;
    case DitherVirtualPixelMethod:
      if (    ( u0 + resample_filter->Ulimit < -32.0 && v0 + resample_filter->Vlimit < -32.0 )
           || ( u0 + resample_filter->Ulimit < -32.0
                && v0 - resample_filter->Vlimit > (double) resample_filter->image->rows+31.0 )
           || ( u0 - resample_filter->Ulimit > (double) resample_filter->image->columns+31.0
                && v0 + resample_filter->Vlimit < -32.0 )
           || ( u0 - resample_filter->Ulimit > (double) resample_filter->image->columns+31.0
                && v0 - resample_filter->Vlimit > (double) resample_filter->image->rows+31.0 )
           )
        hit++;
      break;
    case TileVirtualPixelMethod:
    case MirrorVirtualPixelMethod:
    case RandomVirtualPixelMethod:
    case HorizontalTileEdgeVirtualPixelMethod:
    case VerticalTileEdgeVirtualPixelMethod:
    case CheckerTileVirtualPixelMethod:
      /* resampling of area is always needed - no VP limits */
      break;
  }
  if ( hit ) {
    /* The area being resampled is simply a solid color
     * just return a single lookup color.
     *
     * Should this return the users requested interpolated color?
     */
    status=InterpolatePixelInfo(resample_filter->image,resample_filter->view,
      IntegerInterpolatePixel,u0,v0,pixel,resample_filter->exception);
    return(status);
  }

  /*
    When Scaling limits reached, return an 'averaged' result.
  */
  if ( resample_filter->limit_reached ) {
    switch ( resample_filter->virtual_pixel ) {
      /*  This is always handled by the above, so no need.
        case BackgroundVirtualPixelMethod:
        case ConstantVirtualPixelMethod:
        case TransparentVirtualPixelMethod:
        case GrayVirtualPixelMethod,
        case WhiteVirtualPixelMethod
        case MaskVirtualPixelMethod:
      */
      case UndefinedVirtualPixelMethod:
      case EdgeVirtualPixelMethod:
      case DitherVirtualPixelMethod:
      case HorizontalTileEdgeVirtualPixelMethod:
      case VerticalTileEdgeVirtualPixelMethod:
        /* We need an average edge pixel, from the correct edge!
           How should I calculate an average edge color?
           Just returning an averaged neighbourhood,
           works well in general, but falls down for TileEdge methods.
           This needs to be done properly!!!!!!
        */
        status=InterpolatePixelInfo(resample_filter->image,
          resample_filter->view,AverageInterpolatePixel,u0,v0,pixel,
          resample_filter->exception);
        break;
      case HorizontalTileVirtualPixelMethod:
      case VerticalTileVirtualPixelMethod:
        /* just return the background pixel - Is there more direct way? */
        status=InterpolatePixelInfo(resample_filter->image,
          resample_filter->view,IntegerInterpolatePixel,-1.0,-1.0,pixel,
          resample_filter->exception);
        break;
      case TileVirtualPixelMethod:
      case MirrorVirtualPixelMethod:
      case RandomVirtualPixelMethod:
      case CheckerTileVirtualPixelMethod:
      default:
        /* generate a average color of the WHOLE image */
        if ( resample_filter->average_defined == MagickFalse ) {
          Image
            *average_image;

          CacheView
            *average_view;

          GetPixelInfo(resample_filter->image,(PixelInfo *)
            &resample_filter->average_pixel);
          resample_filter->average_defined=MagickTrue;

          /* Try to get an averaged pixel color of whole image */
          average_image=ResizeImage(resample_filter->image,1,1,BoxFilter,
            resample_filter->exception);
          if (average_image == (Image *) NULL)
            {
              *pixel=resample_filter->average_pixel; /* FAILED */
              break;
            }
          average_view=AcquireVirtualCacheView(average_image,exception);
          pixels=GetCacheViewVirtualPixels(average_view,0,0,1,1,
            resample_filter->exception);
          if (pixels == (const Quantum *) NULL) {
            average_view=DestroyCacheView(average_view);
            average_image=DestroyImage(average_image);
            *pixel=resample_filter->average_pixel; /* FAILED */
            break;
          }
          GetPixelInfoPixel(resample_filter->image,pixels,
            &(resample_filter->average_pixel));
          average_view=DestroyCacheView(average_view);
          average_image=DestroyImage(average_image);

          if ( resample_filter->virtual_pixel == CheckerTileVirtualPixelMethod )
            {
              /* CheckerTile is a alpha blend of the image's average pixel
                 color and the current background color */

              /* image's average pixel color */
              weight = QuantumScale*((double)
                resample_filter->average_pixel.alpha);
              resample_filter->average_pixel.red *= weight;
              resample_filter->average_pixel.green *= weight;
              resample_filter->average_pixel.blue *= weight;
              divisor_c = weight;

              /* background color */
              weight = QuantumScale*((double)
                resample_filter->image->background_color.alpha);
              resample_filter->average_pixel.red +=
                      weight*resample_filter->image->background_color.red;
              resample_filter->average_pixel.green +=
                      weight*resample_filter->image->background_color.green;
              resample_filter->average_pixel.blue +=
                      weight*resample_filter->image->background_color.blue;
              resample_filter->average_pixel.alpha +=
                      resample_filter->image->background_color.alpha;
              divisor_c += weight;

              /* alpha blend */
              resample_filter->average_pixel.red /= divisor_c;
              resample_filter->average_pixel.green /= divisor_c;
              resample_filter->average_pixel.blue /= divisor_c;
              resample_filter->average_pixel.alpha /= 2; /* 50% blend */

            }
        }
        *pixel=resample_filter->average_pixel;
        break;
    }
    return(status);
  }

  /*
    Initialize weighted average data collection
  */
  hit = 0;
  divisor_c = 0.0;
  divisor_m = 0.0;
  pixel->red = pixel->green = pixel->blue = 0.0;
  if (pixel->colorspace == CMYKColorspace)
    pixel->black = 0.0;
  if (pixel->alpha_trait != UndefinedPixelTrait)
    pixel->alpha = 0.0;

  /*
    Determine the parellelogram bounding box fitted to the ellipse
    centered at u0,v0.  This area is bounding by the lines...
  */
  v1 = (ssize_t)ceil(v0 - resample_filter->Vlimit);  /* range of scan lines */
  v2 = (ssize_t)floor(v0 + resample_filter->Vlimit);

  /* scan line start and width accross the parallelogram */
  u1 = u0 + (v1-v0)*resample_filter->slope - resample_filter->Uwidth;
  uw = (ssize_t)(2.0*resample_filter->Uwidth)+1;

#if DEBUG_ELLIPSE
  (void) FormatLocaleFile(stderr, "v1=%ld; v2=%ld\n", (long)v1, (long)v2);
  (void) FormatLocaleFile(stderr, "u1=%ld; uw=%ld\n", (long)u1, (long)uw);
#else
# define DEBUG_HIT_MISS 0 /* only valid if DEBUG_ELLIPSE is enabled */
#endif

  /*
    Do weighted resampling of all pixels,  within the scaled ellipse,
    bound by a Parellelogram fitted to the ellipse.
  */
  DDQ = 2*resample_filter->A;
  for( v=v1; v<=v2;  v++ ) {
#if DEBUG_HIT_MISS
    long uu = ceil(u1);   /* actual pixel location (for debug only) */
    (void) FormatLocaleFile(stderr, "# scan line from pixel %ld, %ld\n", (long)uu, (long)v);
#endif
    u = (ssize_t)ceil(u1);        /* first pixel in scanline */
    u1 += resample_filter->slope; /* start of next scan line */


    /* location of this first pixel, relative to u0,v0 */
    U = (double)u-u0;
    V = (double)v-v0;

    /* Q = ellipse quotent ( if Q<F then pixel is inside ellipse) */
    Q = (resample_filter->A*U + resample_filter->B*V)*U + resample_filter->C*V*V;
    DQ = resample_filter->A*(2.0*U+1) + resample_filter->B*V;

    /* get the scanline of pixels for this v */
    pixels=GetCacheViewVirtualPixels(resample_filter->view,u,v,(size_t) uw,
      1,resample_filter->exception);
    if (pixels == (const Quantum *) NULL)
      return(MagickFalse);

    /* count up the weighted pixel colors */
    for( u=0; u<uw; u++ ) {
#if FILTER_LUT
      /* Note that the ellipse has been pre-scaled so F = WLUT_WIDTH */
      if ( Q < (double)WLUT_WIDTH ) {
        weight = resample_filter->filter_lut[(int)Q];
#else
      /* Note that the ellipse has been pre-scaled so F = support^2 */
      if ( Q < (double)resample_filter->F ) {
        weight = GetResizeFilterWeight(resample_filter->filter_def,
             sqrt(Q));    /* a SquareRoot!  Arrggghhhhh... */
#endif

        pixel->alpha  += weight*GetPixelAlpha(resample_filter->image,pixels);
        divisor_m += weight;

        if (pixel->alpha_trait != UndefinedPixelTrait)
          weight *= QuantumScale*((double) GetPixelAlpha(resample_filter->image,pixels));
        pixel->red   += weight*GetPixelRed(resample_filter->image,pixels);
        pixel->green += weight*GetPixelGreen(resample_filter->image,pixels);
        pixel->blue  += weight*GetPixelBlue(resample_filter->image,pixels);
        if (pixel->colorspace == CMYKColorspace)
          pixel->black += weight*GetPixelBlack(resample_filter->image,pixels);
        divisor_c += weight;

        hit++;
#if DEBUG_HIT_MISS
        /* mark the pixel according to hit/miss of the ellipse */
        (void) FormatLocaleFile(stderr, "set arrow from %lf,%lf to %lf,%lf nohead ls 3\n",
                     (long)uu-.1,(double)v-.1,(long)uu+.1,(long)v+.1);
        (void) FormatLocaleFile(stderr, "set arrow from %lf,%lf to %lf,%lf nohead ls 3\n",
                     (long)uu+.1,(double)v-.1,(long)uu-.1,(long)v+.1);
      } else {
        (void) FormatLocaleFile(stderr, "set arrow from %lf,%lf to %lf,%lf nohead ls 1\n",
                     (long)uu-.1,(double)v-.1,(long)uu+.1,(long)v+.1);
        (void) FormatLocaleFile(stderr, "set arrow from %lf,%lf to %lf,%lf nohead ls 1\n",
                     (long)uu+.1,(double)v-.1,(long)uu-.1,(long)v+.1);
      }
      uu++;
#else
      }
#endif
      pixels+=GetPixelChannels(resample_filter->image);
      Q += DQ;
      DQ += DDQ;
    }
  }
#if DEBUG_ELLIPSE
  (void) FormatLocaleFile(stderr, "Hit=%ld;  Total=%ld;\n", (long)hit, (long)uw*(v2-v1) );
#endif

  /*
    Result sanity check -- this should NOT happen
  */
  if ( hit == 0 || divisor_m <= MagickEpsilon || divisor_c <= MagickEpsilon ) {
    /* not enough pixels, or bad weighting in resampling,
       resort to direct interpolation */
#if DEBUG_NO_PIXEL_HIT
    pixel->alpha = pixel->red = pixel->green = pixel->blue = 0;
    pixel->red = QuantumRange; /* show pixels for which EWA fails */
#else
    status=InterpolatePixelInfo(resample_filter->image,
      resample_filter->view,resample_filter->interpolate,u0,v0,pixel,
      resample_filter->exception);
#endif
    return status;
  }

  /*
    Finialize results of resampling
  */
  divisor_m = 1.0/divisor_m;
  if (pixel->alpha_trait != UndefinedPixelTrait)
    pixel->alpha = (double) ClampToQuantum(divisor_m*pixel->alpha);
  divisor_c = 1.0/divisor_c;
  pixel->red   = (double) ClampToQuantum(divisor_c*pixel->red);
  pixel->green = (double) ClampToQuantum(divisor_c*pixel->green);
  pixel->blue  = (double) ClampToQuantum(divisor_c*pixel->blue);
  if (pixel->colorspace == CMYKColorspace)
    pixel->black = (double) ClampToQuantum(divisor_c*pixel->black);
  return(MagickTrue);
}