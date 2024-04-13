Status AvgPoolGradShape(shape_inference::InferenceContext* c) {
  ShapeHandle s;
  TF_RETURN_IF_ERROR(c->MakeShapeFromShapeTensor(0, &s));
  TF_RETURN_IF_ERROR(c->WithRank(s, 4, &s));
  c->set_output(0, s);
  return Status::OK();
}