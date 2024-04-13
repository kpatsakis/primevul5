Status DepthwiseConv2DNativeShapeWithExplicitPadding(
    shape_inference::InferenceContext* c) {
  return DepthwiseConv2DNativeShapeImpl(c, true);
}