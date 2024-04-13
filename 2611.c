Status QuantizedAvgPoolShape(InferenceContext* c) {
  TF_RETURN_IF_ERROR(shape_inference::AvgPoolShape(c));
  ShapeHandle unused;
  TF_RETURN_IF_ERROR(c->WithRank(c->input(1), 0, &unused));
  TF_RETURN_IF_ERROR(c->WithRank(c->input(2), 0, &unused));
  c->set_output(1, c->Scalar());
  c->set_output(2, c->Scalar());
  return Status::OK();
}