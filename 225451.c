MagickExport Image *AffineTransformImage(const Image *image,
  const AffineMatrix *affine_matrix,ExceptionInfo *exception)
{
  double
    distort[6];

  Image
    *deskew_image;

  /*
    Affine transform image.
  */
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(affine_matrix != (AffineMatrix *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  distort[0]=affine_matrix->sx;
  distort[1]=affine_matrix->rx;
  distort[2]=affine_matrix->ry;
  distort[3]=affine_matrix->sy;
  distort[4]=affine_matrix->tx;
  distort[5]=affine_matrix->ty;
  deskew_image=DistortImage(image,AffineProjectionDistortion,6,distort,
    MagickTrue,exception);
  return(deskew_image);
}