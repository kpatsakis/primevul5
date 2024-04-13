MagickExport MagickBooleanType SetImageExtent(Image *image,const size_t columns,
  const size_t rows)
{
  if ((columns == 0) || (rows == 0))
    ThrowBinaryImageException(ImageError,"NegativeOrZeroImageSize",
      image->filename);
  image->columns=columns;
  image->rows=rows;
  if ((image->depth == 0) || (image->depth > (8*sizeof(MagickSizeType))))
    ThrowBinaryImageException(ImageError,"ImageDepthNotSupported",
      image->filename);
  return(SyncImagePixelCache(image,&image->exception));
}