MagickExport Image *OptimizeImageLayers(const Image *image,
  ExceptionInfo *exception)
{
  return(OptimizeLayerFrames(image,OptimizeImageLayer,exception));
}