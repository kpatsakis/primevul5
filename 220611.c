MagickExport Image *StereoAnaglyphImage(const Image *left_image,
  const Image *right_image,const ssize_t x_offset,const ssize_t y_offset,
  ExceptionInfo *exception)
{
#define StereoImageTag  "Stereo/Image"

  const Image
    *image;

  Image
    *stereo_image;

  MagickBooleanType
    status;

  ssize_t
    y;

  assert(left_image != (const Image *) NULL);
  assert(left_image->signature == MagickCoreSignature);
  if (left_image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      left_image->filename);
  assert(right_image != (const Image *) NULL);
  assert(right_image->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=left_image;
  if ((left_image->columns != right_image->columns) ||
      (left_image->rows != right_image->rows))
    ThrowImageException(ImageError,"LeftAndRightImageSizesDiffer");
  /*
    Initialize stereo image attributes.
  */
  stereo_image=CloneImage(left_image,left_image->columns,left_image->rows,
    MagickTrue,exception);
  if (stereo_image == (Image *) NULL)
    return((Image *) NULL);
  if (SetImageStorageClass(stereo_image,DirectClass,exception) == MagickFalse)
    {
      stereo_image=DestroyImage(stereo_image);
      return((Image *) NULL);
    }
  (void) SetImageColorspace(stereo_image,sRGBColorspace,exception);
  /*
    Copy left image to red channel and right image to blue channel.
  */
  status=MagickTrue;
  for (y=0; y < (ssize_t) stereo_image->rows; y++)
  {
    const Quantum
      *magick_restrict p,
      *magick_restrict q;

    ssize_t
      x;

    Quantum
      *magick_restrict r;

    p=GetVirtualPixels(left_image,-x_offset,y-y_offset,image->columns,1,
      exception);
    q=GetVirtualPixels(right_image,0,y,right_image->columns,1,exception);
    r=QueueAuthenticPixels(stereo_image,0,y,stereo_image->columns,1,exception);
    if ((p == (const Quantum *) NULL) || (q == (Quantum *) NULL) ||
        (r == (Quantum *) NULL))
      break;
    for (x=0; x < (ssize_t) stereo_image->columns; x++)
    {
      SetPixelRed(stereo_image,GetPixelRed(left_image,p),r);
      SetPixelGreen(stereo_image,GetPixelGreen(right_image,q),r);
      SetPixelBlue(stereo_image,GetPixelBlue(right_image,q),r);
      if ((GetPixelAlphaTraits(stereo_image) & CopyPixelTrait) != 0)
        SetPixelAlpha(stereo_image,(GetPixelAlpha(left_image,p)+
          GetPixelAlpha(right_image,q))/2,r);
      p+=GetPixelChannels(left_image);
      q+=GetPixelChannels(right_image);
      r+=GetPixelChannels(stereo_image);
    }
    if (SyncAuthenticPixels(stereo_image,exception) == MagickFalse)
      break;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

        proceed=SetImageProgress(image,StereoImageTag,(MagickOffsetType) y,
          stereo_image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  if (status == MagickFalse)
    stereo_image=DestroyImage(stereo_image);
  return(stereo_image);
}