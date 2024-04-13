static MagickBooleanType WriteSingleWEBPImage(const ImageInfo *image_info,
  Image *image,WebPPicture *picture,PictureMemory *picture_memory,
  ExceptionInfo *exception)
{
  MagickBooleanType
    status = MagickFalse;

  uint32_t
    *magick_restrict q;

  ssize_t
    y;

#if WEBP_ENCODER_ABI_VERSION >= 0x0100
  picture->progress_hook=WebPEncodeProgress;
  picture->user_data=(void *) image;
#endif
  picture->width=(int) image->columns;
  picture->height=(int) image->rows;
  picture->argb_stride=(int) image->columns;
  picture->use_argb=1;

  /*
    Allocate memory for pixels.
  */
  (void) TransformImageColorspace(image,sRGBColorspace);
  picture_memory->pixel_info=AcquireVirtualMemory(image->columns,image->rows*
    sizeof(*(picture->argb)));

  if (picture_memory->pixel_info == (MemoryInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  picture->argb=(uint32_t *) GetVirtualMemoryBlob(picture_memory->pixel_info);
  /*
    Convert image to WebP raster pixels.
  */
  q=picture->argb;
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    const PixelPacket
      *magick_restrict p;

    ssize_t
      x;

    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const PixelPacket *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      *q++=(uint32_t) (image->matte != MagickFalse ? (uint32_t)
        ScaleQuantumToChar(GetPixelAlpha(p)) << 24 : 0xff000000) |
        ((uint32_t) ScaleQuantumToChar(GetPixelRed(p)) << 16) |
        ((uint32_t) ScaleQuantumToChar(GetPixelGreen(p)) << 8) |
        ((uint32_t) ScaleQuantumToChar(GetPixelBlue(p)));
      p++;
    }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  return status;
}