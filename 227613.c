MagickExport void SetResampleFilter(ResampleFilter *resample_filter,
  const FilterType filter)
{
  ResizeFilter
     *resize_filter;

  assert(resample_filter != (ResampleFilter *) NULL);
  assert(resample_filter->signature == MagickCoreSignature);

  resample_filter->do_interpolate = MagickFalse;
  resample_filter->filter = filter;

  /* Default cylindrical filter is a Cubic Keys filter */
  if ( filter == UndefinedFilter )
    resample_filter->filter = RobidouxFilter;

  if ( resample_filter->filter == PointFilter ) {
    resample_filter->do_interpolate = MagickTrue;
    return;  /* EWA turned off - nothing more to do */
  }

  resize_filter = AcquireResizeFilter(resample_filter->image,
    resample_filter->filter,MagickTrue,resample_filter->exception);
  if (resize_filter == (ResizeFilter *) NULL) {
    (void) ThrowMagickException(resample_filter->exception,GetMagickModule(),
         ModuleError, "UnableToSetFilteringValue",
         "Fall back to Interpolated 'Point' filter");
    resample_filter->filter = PointFilter;
    resample_filter->do_interpolate = MagickTrue;
    return;  /* EWA turned off - nothing more to do */
  }

  /* Get the practical working support for the filter,
   * after any API call blur factors have been accoded for.
   */
#if EWA
  resample_filter->support = GetResizeFilterSupport(resize_filter);
#else
  resample_filter->support = 2.0;  /* fixed support size for HQ-EWA */
#endif

#if FILTER_LUT
  /* Fill the LUT with the weights from the selected filter function */
  { int
       Q;
    double
       r_scale;

    /* Scale radius so the filter LUT covers the full support range */
    r_scale = resample_filter->support*sqrt(1.0/(double)WLUT_WIDTH);
    for(Q=0; Q<WLUT_WIDTH; Q++)
      resample_filter->filter_lut[Q] = (double)
           GetResizeFilterWeight(resize_filter,sqrt((double)Q)*r_scale);

    /* finished with the resize filter */
    resize_filter = DestroyResizeFilter(resize_filter);
  }
#else
  /* save the filter and the scaled ellipse bounds needed for filter */
  resample_filter->filter_def = resize_filter;
  resample_filter->F = resample_filter->support*resample_filter->support;
#endif

  /*
    Adjust the scaling of the default unit circle
    This assumes that any real scaling changes will always
    take place AFTER the filter method has been initialized.
  */
  ScaleResampleFilter(resample_filter, 1.0, 0.0, 0.0, 1.0);

#if 0
  /*
    This is old code kept as a reference only. Basically it generates
    a Gaussian bell curve, with sigma = 0.5 if the support is 2.0

    Create Normal Gaussian 2D Filter Weighted Lookup Table.
    A normal EWA guassual lookup would use   exp(Q*ALPHA)
    where  Q = distance squared from 0.0 (center) to 1.0 (edge)
    and    ALPHA = -4.0*ln(2.0)  ==>  -2.77258872223978123767
    The table is of length 1024, and equates to support radius of 2.0
    thus needs to be scaled by  ALPHA*4/1024 and any blur factor squared

    The it comes from reference code provided by Fred Weinhaus.
  */
  r_scale = -2.77258872223978123767/(WLUT_WIDTH*blur*blur);
  for(Q=0; Q<WLUT_WIDTH; Q++)
    resample_filter->filter_lut[Q] = exp((double)Q*r_scale);
  resample_filter->support = WLUT_WIDTH;
#endif

#if FILTER_LUT
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp single
#endif
  {
    if (IsStringTrue(GetImageArtifact(resample_filter->image,
        "resample:verbose")) != MagickFalse)
      {
        int
          Q;
        double
          r_scale;

        /* Debug output of the filter weighting LUT
          Gnuplot the LUT data, the x scale index has been adjusted
            plot [0:2][-.2:1] "lut.dat" with lines
          The filter values should be normalized for comparision
        */
        printf("#\n");
        printf("# Resampling Filter LUT (%d values) for '%s' filter\n",
                   WLUT_WIDTH, CommandOptionToMnemonic(MagickFilterOptions,
                   resample_filter->filter) );
        printf("#\n");
        printf("# Note: values in table are using a squared radius lookup.\n");
        printf("# As such its distribution is not uniform.\n");
        printf("#\n");
        printf("# The X value is the support distance for the Y weight\n");
        printf("# so you can use gnuplot to plot this cylindrical filter\n");
        printf("#    plot [0:2][-.2:1] \"lut.dat\" with lines\n");
        printf("#\n");

        /* Scale radius so the filter LUT covers the full support range */
        r_scale = resample_filter->support*sqrt(1.0/(double)WLUT_WIDTH);
        for(Q=0; Q<WLUT_WIDTH; Q++)
          printf("%8.*g %.*g\n",
              GetMagickPrecision(),sqrt((double)Q)*r_scale,
              GetMagickPrecision(),resample_filter->filter_lut[Q] );
        printf("\n\n"); /* generate a 'break' in gnuplot if multiple outputs */
      }
    /* Output the above once only for each image, and each setting
    (void) DeleteImageArtifact(resample_filter->image,"resample:verbose");
    */
  }
#endif /* FILTER_LUT */
  return;
}