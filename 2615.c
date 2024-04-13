Status MaxPoolShape(shape_inference::InferenceContext* c) {
  return MaxPoolShapeImpl(c, /*supports_explicit_padding=*/false);
}