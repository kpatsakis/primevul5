Status FusedBatchNormV3Shape(shape_inference::InferenceContext* c) {
  TF_RETURN_IF_ERROR(FusedBatchNormShape(c));
  c->set_output(5, c->UnknownShape());
  return Status::OK();
}