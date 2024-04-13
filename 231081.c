MagickExport void GetImageInfo(ImageInfo *image_info)
{
  char
    *synchronize;

  ExceptionInfo
    *exception;

  /*
    File and image dimension members.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(image_info != (ImageInfo *) NULL);
  (void) memset(image_info,0,sizeof(*image_info));
  image_info->adjoin=MagickTrue;
  image_info->interlace=NoInterlace;
  image_info->channel=DefaultChannels;
  image_info->quality=UndefinedCompressionQuality;
  image_info->antialias=MagickTrue;
  image_info->dither=MagickTrue;
  synchronize=GetEnvironmentValue("MAGICK_SYNCHRONIZE");
  if (synchronize != (const char *) NULL)
    {
      image_info->synchronize=IsStringTrue(synchronize);
      synchronize=DestroyString(synchronize);
    }
  exception=AcquireExceptionInfo();
  (void) QueryColorDatabase(BackgroundColor,&image_info->background_color,
    exception);
  (void) QueryColorDatabase(BorderColor,&image_info->border_color,exception);
  (void) QueryColorDatabase(MatteColor,&image_info->matte_color,exception);
  (void) QueryColorDatabase(TransparentColor,&image_info->transparent_color,
    exception);
  exception=DestroyExceptionInfo(exception);
  image_info->debug=IsEventLogging();
  image_info->signature=MagickCoreSignature;
}