bool IsShapeFullyDefinedIntegerVectorOrScalar(
    InferenceContext* ic, const ShapeHandle& shape,
    const ShapeHandle& tensor_as_shape, const DataType& dtype) {
  if (!ic->FullyDefined(shape) || ic->Rank(shape) > 1 ||
      !ic->FullyDefined(tensor_as_shape) ||
      (dtype != DT_INT32 && dtype != DT_INT64)) {
    return false;
  }
  // Also check whether any dim in tensor_as_shape is kUnknownDimFromConst.
  for (int32_t i = 0; i < ic->Rank(tensor_as_shape); ++i) {
    DimensionHandle dim = ic->Dim(tensor_as_shape, i);
    if (ic->Value(dim) == kUnknownDimFromConst) {
      LOG(WARNING) << "IsShapeFullyDefinedIntegerVectorOrScalar(): "
                   << "tensor_as_shape input includes kUnknownDimFromConst -- "
                   << ic->DebugString(tensor_as_shape);
      return false;
    }
  }
  return true;
}