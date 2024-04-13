Status MaxPool3DGradShape(shape_inference::InferenceContext* c) {
  return UnchangedShapeWithRank(c, 5);
}