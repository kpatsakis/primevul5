MagickExport Image *DistortResizeImage(const Image *image,const size_t columns,
  const size_t rows,ExceptionInfo *exception)
{
#define DistortResizeImageTag  "Distort/Image"

  Image
    *resize_image,
    *tmp_image;

  RectangleInfo
    crop_area;

  double
    distort_args[12];

  VirtualPixelMethod
    vp_save;

  /*
    Distort resize image.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if ((columns == 0) || (rows == 0))
    return((Image *) NULL);
  /* Do not short-circuit this resize if final image size is unchanged */

  (void) memset(distort_args,0,sizeof(distort_args));
  distort_args[4]=(double) image->columns;
  distort_args[6]=(double) columns;
  distort_args[9]=(double) image->rows;
  distort_args[11]=(double) rows;

  vp_save=GetImageVirtualPixelMethod(image);

  tmp_image=CloneImage(image,0,0,MagickTrue,exception);
  if (tmp_image == (Image *) NULL)
    return((Image *) NULL);
  (void) SetImageVirtualPixelMethod(tmp_image,TransparentVirtualPixelMethod,
    exception);

  if (image->alpha_trait == UndefinedPixelTrait)
    {
      /*
        Image has no alpha channel, so we are free to use it.
      */
      (void) SetImageAlphaChannel(tmp_image,SetAlphaChannel,exception);
      resize_image=DistortImage(tmp_image,AffineDistortion,12,distort_args,
        MagickTrue,exception),
      tmp_image=DestroyImage(tmp_image);
      if (resize_image == (Image *) NULL)
        return((Image *) NULL);
      (void) SetImageAlphaChannel(resize_image,OffAlphaChannel,exception);
    }
  else
    {
      /*
        Image has transparency so handle colors and alpha separatly.
        Basically we need to separate Virtual-Pixel alpha in the resized
        image, so only the actual original images alpha channel is used.

        distort alpha channel separately
      */
      Image
        *resize_alpha;

      (void) SetImageAlphaChannel(tmp_image,ExtractAlphaChannel,exception);
      (void) SetImageAlphaChannel(tmp_image,OpaqueAlphaChannel,exception);
      resize_alpha=DistortImage(tmp_image,AffineDistortion,12,distort_args,
        MagickTrue,exception),
      tmp_image=DestroyImage(tmp_image);
      if (resize_alpha == (Image *) NULL)
        return((Image *) NULL);

      /* distort the actual image containing alpha + VP alpha */
      tmp_image=CloneImage(image,0,0,MagickTrue,exception);
      if (tmp_image == (Image *) NULL)
        return((Image *) NULL);
      (void) SetImageVirtualPixelMethod(tmp_image,
        TransparentVirtualPixelMethod,exception);
      resize_image=DistortImage(tmp_image,AffineDistortion,12,distort_args,
        MagickTrue,exception),
      tmp_image=DestroyImage(tmp_image);
      if (resize_image == (Image *) NULL)
        {
          resize_alpha=DestroyImage(resize_alpha);
          return((Image *) NULL);
        }
      /* replace resize images alpha with the separally distorted alpha */
      (void) SetImageAlphaChannel(resize_image,OffAlphaChannel,exception);
      (void) SetImageAlphaChannel(resize_alpha,OffAlphaChannel,exception);
      (void) CompositeImage(resize_image,resize_alpha,CopyAlphaCompositeOp,
        MagickTrue,0,0,exception);
      resize_alpha=DestroyImage(resize_alpha);
      resize_image->alpha_trait=image->alpha_trait;
      resize_image->compose=image->compose;
    }
  (void) SetImageVirtualPixelMethod(resize_image,vp_save,exception);

  /*
    Clean up the results of the Distortion
  */
  crop_area.width=columns;
  crop_area.height=rows;
  crop_area.x=0;
  crop_area.y=0;

  tmp_image=resize_image;
  resize_image=CropImage(tmp_image,&crop_area,exception);
  tmp_image=DestroyImage(tmp_image);
  if (resize_image != (Image *) NULL)
    {
      resize_image->page.width=0;
      resize_image->page.height=0;
    }
  return(resize_image);
}