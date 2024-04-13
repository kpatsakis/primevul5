Status Conv2DShape(shape_inference::InferenceContext* c) {
  return Conv2DShapeImpl(c, false);
}