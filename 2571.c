Status MaxPoolGradShape(shape_inference::InferenceContext* c) {
  return UnchangedShapeWithRank(c, 4);
}