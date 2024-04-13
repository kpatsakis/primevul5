Status QuantizedConv2DShape(InferenceContext* c) {
  TF_RETURN_IF_ERROR(shape_inference::Conv2DShape(c));
  ShapeHandle unused;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(2), 0, &unused));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(3), 0, &unused));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(4), 0, &unused));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(5), 0, &unused));
  c->set_output(1, c->Scalar());
  c->set_output(2, c->Scalar());
  return Status::OK();
}