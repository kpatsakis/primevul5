static void ClearBounds(Image *image,RectangleInfo *bounds,
  ExceptionInfo *exception)
{
  ssize_t
    y;

  if (bounds->x < 0)
    return;
  if (image->alpha_trait == UndefinedPixelTrait)
    (void) SetImageAlphaChannel(image,OpaqueAlphaChannel,exception);
  for (y=0; y < (ssize_t) bounds->height; y++)
  {
    register ssize_t
      x;

    register Quantum
      *magick_restrict q;

    q=GetAuthenticPixels(image,bounds->x,bounds->y+y,bounds->width,1,exception);
    if (q == (Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) bounds->width; x++)
    {
      SetPixelAlpha(image,TransparentAlpha,q);
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      break;
  }
}