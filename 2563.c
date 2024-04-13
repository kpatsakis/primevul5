Status GetWindowedOutputSizeFromDims(
    shape_inference::InferenceContext* c,
    shape_inference::DimensionHandle input_size,
    shape_inference::DimensionOrConstant filter_size, int64_t stride,
    Padding padding_type, shape_inference::DimensionHandle* output_size) {
  if (padding_type == Padding::EXPLICIT) {
    return errors::Internal(
        "GetWindowedOutputSizeFromDims does not handle EXPLICIT padding; call "
        "GetWindowedOutputSizeFromDimsV2 instead");
  }
  return GetWindowedOutputSizeFromDimsV2(c, input_size, filter_size,
                                         /*dilation_rate=*/1, stride,
                                         padding_type,
                                         // Give dummy values of -1 to
                                         // padding_before and padding_after,
                                         // since explicit padding is not used.
                                         -1, -1, output_size);
}