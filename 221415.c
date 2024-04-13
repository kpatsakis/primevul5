MagickExport Image *OptimizePlusImageLayers(const Image *image,
  ExceptionInfo *exception)
{
  return OptimizeLayerFrames(image,OptimizePlusLayer,exception);
}