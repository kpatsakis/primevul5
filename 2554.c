Status GetWindowedOutputSizeFromDimsV2(
    shape_inference::InferenceContext* c,
    shape_inference::DimensionHandle input_size,
    shape_inference::DimensionOrConstant filter_size, int64_t dilation_rate,
    int64_t stride, Padding padding_type, int64_t padding_before,
    int64_t padding_after, shape_inference::DimensionHandle* output_size) {
  if (stride <= 0) {
    return errors::InvalidArgument("Stride must be > 0, but got ", stride);
  }

  if (dilation_rate < 1) {
    return errors::InvalidArgument("Dilation rate must be >= 1, but got ",
                                   dilation_rate);
  }

  // See also the parallel implementation in GetWindowedOutputSizeVerbose.
  switch (padding_type) {
    case Padding::VALID:
      padding_before = padding_after = 0;
      TF_FALLTHROUGH_INTENDED;
    case Padding::EXPLICIT:
      TF_RETURN_IF_ERROR(
          c->Add(input_size, padding_before + padding_after, &input_size));
      if (dilation_rate > 1) {
        DimensionHandle window_size;
        TF_RETURN_IF_ERROR(
            c->Subtract(c->MakeDim(filter_size), 1, &window_size));
        TF_RETURN_IF_ERROR(
            c->Multiply(window_size, dilation_rate, &window_size));
        TF_RETURN_IF_ERROR(c->Add(window_size, 1, &window_size));
        TF_RETURN_IF_ERROR(c->Subtract(input_size, window_size, output_size));
      } else {
        TF_RETURN_IF_ERROR(c->Subtract(input_size, filter_size, output_size));
      }
      TF_RETURN_IF_ERROR(c->Add(*output_size, stride, output_size));
      TF_RETURN_IF_ERROR(c->Divide(*output_size, stride,
                                   /*evenly_divisible=*/false, output_size));
      break;
    case Padding::SAME:
      TF_RETURN_IF_ERROR(c->Add(input_size, stride - 1, output_size));
      TF_RETURN_IF_ERROR(c->Divide(*output_size, stride,
                                   /*evenly_divisible=*/false, output_size));
      break;
  }
  return Status::OK();
}