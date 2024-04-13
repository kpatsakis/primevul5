Status MaxPoolShapeWithExplicitPadding(shape_inference::InferenceContext* c) {
  return MaxPoolShapeImpl(c, /*supports_explicit_padding=*/true);
}