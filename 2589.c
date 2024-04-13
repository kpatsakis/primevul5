Status ExplicitShape(InferenceContext* c) {
  PartialTensorShape shape;
  TF_RETURN_IF_ERROR(c->GetAttr("shape", &shape));
  ShapeHandle output_shape;
  TF_RETURN_IF_ERROR(c->MakeShapeFromPartialTensorShape(shape, &output_shape));
  c->set_output(0, output_shape);
  return Status::OK();
}