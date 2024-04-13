Status Conv2DShapeWithExplicitPadding(shape_inference::InferenceContext* c) {
  return Conv2DShapeImpl(c, true);
}