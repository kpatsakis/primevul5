MagickExport Image *AcquireImage(const ImageInfo *image_info)
{
  const char
    *option;

  Image
    *image;

  MagickStatusType
    flags;

  /*
    Allocate image structure.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  image=(Image *) AcquireCriticalMemory(sizeof(*image));
  (void) memset(image,0,sizeof(*image));
  /*
    Initialize Image structure.
  */
  (void) CopyMagickString(image->magick,"MIFF",MaxTextExtent);
  image->storage_class=DirectClass;
  image->depth=MAGICKCORE_QUANTUM_DEPTH;
  image->colorspace=sRGBColorspace;
  image->rendering_intent=PerceptualIntent;
  image->gamma=1.000f/2.200f;
  image->chromaticity.red_primary.x=0.6400f;
  image->chromaticity.red_primary.y=0.3300f;
  image->chromaticity.red_primary.z=0.0300f;
  image->chromaticity.green_primary.x=0.3000f;
  image->chromaticity.green_primary.y=0.6000f;
  image->chromaticity.green_primary.z=0.1000f;
  image->chromaticity.blue_primary.x=0.1500f;
  image->chromaticity.blue_primary.y=0.0600f;
  image->chromaticity.blue_primary.z=0.7900f;
  image->chromaticity.white_point.x=0.3127f;
  image->chromaticity.white_point.y=0.3290f;
  image->chromaticity.white_point.z=0.3583f;
  image->interlace=NoInterlace;
  image->ticks_per_second=UndefinedTicksPerSecond;
  image->compose=OverCompositeOp;
  image->blur=1.0;
  InitializeExceptionInfo(&image->exception);
  (void) QueryColorDatabase(BackgroundColor,&image->background_color,
    &image->exception);
  (void) QueryColorDatabase(BorderColor,&image->border_color,&image->exception);
  (void) QueryColorDatabase(MatteColor,&image->matte_color,&image->exception);
  (void) QueryColorDatabase(TransparentColor,&image->transparent_color,
    &image->exception);
  GetTimerInfo(&image->timer);
  image->ping=MagickFalse;
  image->cache=AcquirePixelCache(0);
  image->blob=CloneBlobInfo((BlobInfo *) NULL);
  image->timestamp=time((time_t *) NULL);
  image->debug=IsEventLogging();
  image->reference_count=1;
  image->semaphore=AllocateSemaphoreInfo();
  image->signature=MagickCoreSignature;
  if (image_info == (ImageInfo *) NULL)
    return(image);
  /*
    Transfer image info.
  */
  SetBlobExempt(image,image_info->file != (FILE *) NULL ? MagickTrue :
    MagickFalse);
  (void) CopyMagickString(image->filename,image_info->filename,MaxTextExtent);
  (void) CopyMagickString(image->magick_filename,image_info->filename,
    MaxTextExtent);
  (void) CopyMagickString(image->magick,image_info->magick,MaxTextExtent);
  if (image_info->size != (char *) NULL)
    {
      (void) ParseAbsoluteGeometry(image_info->size,&image->extract_info);
      image->columns=image->extract_info.width;
      image->rows=image->extract_info.height;
      image->offset=image->extract_info.x;
      image->extract_info.x=0;
      image->extract_info.y=0;
    }
  if (image_info->extract != (char *) NULL)
    {
      RectangleInfo
        geometry;

      (void) memset(&geometry,0,sizeof(geometry));
      flags=ParseAbsoluteGeometry(image_info->extract,&geometry);
      if (((flags & XValue) != 0) || ((flags & YValue) != 0))
        {
          image->extract_info=geometry;
          Swap(image->columns,image->extract_info.width);
          Swap(image->rows,image->extract_info.height);
        }
    }
  image->compression=image_info->compression;
  image->quality=image_info->quality;
  image->endian=image_info->endian;
  image->interlace=image_info->interlace;
  image->units=image_info->units;
  if (image_info->density != (char *) NULL)
    {
      GeometryInfo
        geometry_info;

      flags=ParseGeometry(image_info->density,&geometry_info);
      if ((flags & RhoValue) != 0)
        image->x_resolution=geometry_info.rho;
      image->y_resolution=image->x_resolution;
      if ((flags & SigmaValue) != 0)
        image->y_resolution=geometry_info.sigma;
    }
  if (image_info->page != (char *) NULL)
    {
      char
        *geometry;

      image->page=image->extract_info;
      geometry=GetPageGeometry(image_info->page);
      (void) ParseAbsoluteGeometry(geometry,&image->page);
      geometry=DestroyString(geometry);
    }
  if (image_info->depth != 0)
    image->depth=image_info->depth;
  image->dither=image_info->dither;
  image->background_color=image_info->background_color;
  image->border_color=image_info->border_color;
  image->matte_color=image_info->matte_color;
  image->transparent_color=image_info->transparent_color;
  image->ping=image_info->ping;
  image->progress_monitor=image_info->progress_monitor;
  image->client_data=image_info->client_data;
  if (image_info->cache != (void *) NULL)
    ClonePixelCacheMethods(image->cache,image_info->cache);
  (void) SyncImageSettings(image_info,image);
  option=GetImageOption(image_info,"delay");
  if (option != (const char *) NULL)
    {
      GeometryInfo
        geometry_info;

      flags=ParseGeometry(option,&geometry_info);
      if ((flags & GreaterValue) != 0)
        {
          if (image->delay > (size_t) floor(geometry_info.rho+0.5))
            image->delay=(size_t) floor(geometry_info.rho+0.5);
        }
      else
        if ((flags & LessValue) != 0)
          {
            if (image->delay < (size_t) floor(geometry_info.rho+0.5))
              image->ticks_per_second=(ssize_t) floor(geometry_info.sigma+0.5);
          }
        else
          image->delay=(size_t) floor(geometry_info.rho+0.5);
      if ((flags & SigmaValue) != 0)
        image->ticks_per_second=(ssize_t) floor(geometry_info.sigma+0.5);
    }
  option=GetImageOption(image_info,"dispose");
  if (option != (const char *) NULL)
    image->dispose=(DisposeType) ParseCommandOption(MagickDisposeOptions,
      MagickFalse,option);
  return(image);
}