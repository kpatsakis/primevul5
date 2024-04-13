MagickExport Image *MeanShiftImage(const Image *image,const size_t width,
  const size_t height,const double color_distance,ExceptionInfo *exception)
{
#define MaxMeanShiftIterations  100
#define MeanShiftImageTag  "MeanShift/Image"

  CacheView
    *image_view,
    *mean_view,
    *pixel_view;

  Image
    *mean_image;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  ssize_t
    y;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  mean_image=CloneImage(image,0,0,MagickTrue,exception);
  if (mean_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(mean_image,DirectClass,exception) == MagickFalse)
    {
      mean_image=DestroyImage(mean_image);
      return((Image *) NULL);
    }
  status=MagickTrue;
  progress=0;
  image_view=AcquireVirtualCacheView(image,exception);
  pixel_view=AcquireVirtualCacheView(image,exception);
  mean_view=AcquireAuthenticCacheView(mean_image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(status,progress) \
    magick_number_threads(mean_image,mean_image,mean_image->rows,1)
#endif
  for (y=0; y < (ssize_t) mean_image->rows; y++)
  {
    register const Quantum
      *magick_restrict p;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    q=GetCacheViewAuthenticPixels(mean_view,0,y,mean_image->columns,1,
      exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL))
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) mean_image->columns; x++)
    {
      PixelInfo
        mean_pixel,
        previous_pixel;

      PointInfo
        mean_location,
        previous_location;

      register ssize_t
        i;

      GetPixelInfo(image,&mean_pixel);
      GetPixelInfoPixel(image,p,&mean_pixel);
      mean_location.x=(double) x;
      mean_location.y=(double) y;
      for (i=0; i < MaxMeanShiftIterations; i++)
      {
        double
          distance,
          gamma;

        PixelInfo
          sum_pixel;

        PointInfo
          sum_location;

        ssize_t
          count,
          v;

        sum_location.x=0.0;
        sum_location.y=0.0;
        GetPixelInfo(image,&sum_pixel);
        previous_location=mean_location;
        previous_pixel=mean_pixel;
        count=0;
        for (v=(-((ssize_t) height/2)); v <= (((ssize_t) height/2)); v++)
        {
          ssize_t
            u;

          for (u=(-((ssize_t) width/2)); u <= (((ssize_t) width/2)); u++)
          {
            if ((v*v+u*u) <= (ssize_t) ((width/2)*(height/2)))
              {
                PixelInfo
                  pixel;

                status=GetOneCacheViewVirtualPixelInfo(pixel_view,(ssize_t)
                  MagickRound(mean_location.x+u),(ssize_t) MagickRound(
                  mean_location.y+v),&pixel,exception);
                distance=(mean_pixel.red-pixel.red)*(mean_pixel.red-pixel.red)+
                  (mean_pixel.green-pixel.green)*(mean_pixel.green-pixel.green)+
                  (mean_pixel.blue-pixel.blue)*(mean_pixel.blue-pixel.blue);
                if (distance <= (color_distance*color_distance))
                  {
                    sum_location.x+=mean_location.x+u;
                    sum_location.y+=mean_location.y+v;
                    sum_pixel.red+=pixel.red;
                    sum_pixel.green+=pixel.green;
                    sum_pixel.blue+=pixel.blue;
                    sum_pixel.alpha+=pixel.alpha;
                    count++;
                  }
               }
           }
         }
        gamma=1.0/count;
         mean_location.x=gamma*sum_location.x;
         mean_location.y=gamma*sum_location.y;
         mean_pixel.red=gamma*sum_pixel.red;
        mean_pixel.green=gamma*sum_pixel.green;
        mean_pixel.blue=gamma*sum_pixel.blue;
        mean_pixel.alpha=gamma*sum_pixel.alpha;
        distance=(mean_location.x-previous_location.x)*
          (mean_location.x-previous_location.x)+
          (mean_location.y-previous_location.y)*
          (mean_location.y-previous_location.y)+
          255.0*QuantumScale*(mean_pixel.red-previous_pixel.red)*
          255.0*QuantumScale*(mean_pixel.red-previous_pixel.red)+
          255.0*QuantumScale*(mean_pixel.green-previous_pixel.green)*
          255.0*QuantumScale*(mean_pixel.green-previous_pixel.green)+
          255.0*QuantumScale*(mean_pixel.blue-previous_pixel.blue)*
          255.0*QuantumScale*(mean_pixel.blue-previous_pixel.blue);
        if (distance <= 3.0)
          break;
      }
      SetPixelRed(mean_image,ClampToQuantum(mean_pixel.red),q);
      SetPixelGreen(mean_image,ClampToQuantum(mean_pixel.green),q);
      SetPixelBlue(mean_image,ClampToQuantum(mean_pixel.blue),q);
      SetPixelAlpha(mean_image,ClampToQuantum(mean_pixel.alpha),q);
      p+=GetPixelChannels(image);
      q+=GetPixelChannels(mean_image);
    }
    if (SyncCacheViewAuthenticPixels(mean_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,MeanShiftImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  mean_view=DestroyCacheView(mean_view);
  pixel_view=DestroyCacheView(pixel_view);
  image_view=DestroyCacheView(image_view);
  return(mean_image);
}
