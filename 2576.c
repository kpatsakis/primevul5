Status DepthwiseConv2DNativeShape(shape_inference::InferenceContext* c) {
  return DepthwiseConv2DNativeShapeImpl(c, false);
}