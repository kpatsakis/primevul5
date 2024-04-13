MagickExport Image *PolaroidImage(const Image *image,const DrawInfo *draw_info,
  const double angle,ExceptionInfo *exception)
{
  const char
    *value;

  Image
    *bend_image,
    *caption_image,
    *flop_image,
    *picture_image,
    *polaroid_image,
    *rotate_image,
    *trim_image;

  size_t
    height;

  ssize_t
    quantum;

  /*
    Simulate a Polaroid picture.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  quantum=(ssize_t) MagickMax(MagickMax((double) image->columns,(double)
    image->rows)/25.0,10.0);
  height=image->rows+2*quantum;
  caption_image=(Image *) NULL;
  value=GetImageProperty(image,"Caption");
  if (value != (const char *) NULL)
    {
      char
        *caption;

      /*
        Generate caption image.
      */
      caption_image=CloneImage(image,image->columns,1,MagickTrue,exception);
      if (caption_image == (Image *) NULL)
        return((Image *) NULL);
      caption=InterpretImageProperties((ImageInfo *) NULL,(Image *) image,
        value);
      if (caption != (char *) NULL)
        {
          char
            geometry[MaxTextExtent];

          DrawInfo
            *annotate_info;

          MagickBooleanType
            status;

          ssize_t
            count;

          TypeMetric
            metrics;

          annotate_info=CloneDrawInfo((const ImageInfo *) NULL,draw_info);
          (void) CloneString(&annotate_info->text,caption);
          count=FormatMagickCaption(caption_image,annotate_info,MagickTrue,
            &metrics,&caption);
          status=SetImageExtent(caption_image,image->columns,(size_t)
            ((count+1)*(metrics.ascent-metrics.descent)+0.5));
          if (status == MagickFalse)
            caption_image=DestroyImage(caption_image);
          else
            {
              caption_image->background_color=image->border_color;
              (void) SetImageBackgroundColor(caption_image);
              (void) CloneString(&annotate_info->text,caption);
              (void) FormatLocaleString(geometry,MaxTextExtent,"+0+%.20g",
                metrics.ascent);
              if (annotate_info->gravity == UndefinedGravity)
                (void) CloneString(&annotate_info->geometry,AcquireString(
                  geometry));
              (void) AnnotateImage(caption_image,annotate_info);
              height+=caption_image->rows;
            }
          annotate_info=DestroyDrawInfo(annotate_info);
          caption=DestroyString(caption);
        }
    }
  picture_image=CloneImage(image,image->columns+2*quantum,height,MagickTrue,
    exception);
  if (picture_image == (Image *) NULL)
    {
      if (caption_image != (Image *) NULL)
        caption_image=DestroyImage(caption_image);
      return((Image *) NULL);
    }
  picture_image->background_color=image->border_color;
  (void) SetImageBackgroundColor(picture_image);
  (void) CompositeImage(picture_image,OverCompositeOp,image,quantum,quantum);
  if (caption_image != (Image *) NULL)
    {
      (void) CompositeImage(picture_image,OverCompositeOp,caption_image,
        quantum,(ssize_t) (image->rows+3*quantum/2));
      caption_image=DestroyImage(caption_image);
    }
  (void) QueryColorDatabase("none",&picture_image->background_color,exception);
  (void) SetImageAlphaChannel(picture_image,OpaqueAlphaChannel);
  rotate_image=RotateImage(picture_image,90.0,exception);
  picture_image=DestroyImage(picture_image);
  if (rotate_image == (Image *) NULL)
    return((Image *) NULL);
  picture_image=rotate_image;
  bend_image=WaveImage(picture_image,0.01*picture_image->rows,2.0*
    picture_image->columns,exception);
  picture_image=DestroyImage(picture_image);
  if (bend_image == (Image *) NULL)
    return((Image *) NULL);
  InheritException(&bend_image->exception,exception);
  picture_image=bend_image;
  rotate_image=RotateImage(picture_image,-90.0,exception);
  picture_image=DestroyImage(picture_image);
  if (rotate_image == (Image *) NULL)
    return((Image *) NULL);
  picture_image=rotate_image;
  picture_image->background_color=image->background_color;
  polaroid_image=ShadowImage(picture_image,80.0,2.0,quantum/3,quantum/3,
    exception);
  if (polaroid_image == (Image *) NULL)
    {
      picture_image=DestroyImage(picture_image);
      return(picture_image);
    }
  flop_image=FlopImage(polaroid_image,exception);
  polaroid_image=DestroyImage(polaroid_image);
  if (flop_image == (Image *) NULL)
    {
      picture_image=DestroyImage(picture_image);
      return(picture_image);
    }
  polaroid_image=flop_image;
  (void) CompositeImage(polaroid_image,OverCompositeOp,picture_image,
    (ssize_t) (-0.01*picture_image->columns/2.0),0L);
  picture_image=DestroyImage(picture_image);
  (void) QueryColorDatabase("none",&polaroid_image->background_color,exception);
  rotate_image=RotateImage(polaroid_image,angle,exception);
  polaroid_image=DestroyImage(polaroid_image);
  if (rotate_image == (Image *) NULL)
    return((Image *) NULL);
  polaroid_image=rotate_image;
  trim_image=TrimImage(polaroid_image,exception);
  polaroid_image=DestroyImage(polaroid_image);
  if (trim_image == (Image *) NULL)
    return((Image *) NULL);
  polaroid_image=trim_image;
  return(polaroid_image);
}