Status Conv2DBackpropFilterWithBiasShape(shape_inference::InferenceContext* c) {
  ShapeHandle input_shape;
  // Fetch the data_format attribute, which may not exist.
  string data_format;
  Status s = c->GetAttr("data_format", &data_format);

  TF_RETURN_IF_ERROR(c->WithRank(c->input(0), 4, &input_shape));
  if (s.ok() && data_format == "NCHW") {
    c->set_output(1, c->Vector(c->Dim(input_shape, -3)));
  } else {
    c->set_output(1, c->Vector(c->Dim(input_shape, -1)));
  }
  ShapeHandle sh;
  TF_RETURN_IF_ERROR(c->MakeShapeFromShapeTensor(1, &sh));
  TF_RETURN_IF_ERROR(c->WithRank(sh, 4, &sh));
  c->set_output(0, sh);
  return Status::OK();
}