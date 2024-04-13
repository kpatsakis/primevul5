Status ExplicitShapes(InferenceContext* c) {
  std::vector<PartialTensorShape> shapes;
  TF_RETURN_IF_ERROR(c->GetAttr("shapes", &shapes));
  if (shapes.empty()) {
    return errors::Internal("shapes attribute is empty");
  }
  for (int i = 0, end = shapes.size(); i < end; ++i) {
    ShapeHandle output_shape;
    TF_RETURN_IF_ERROR(
        c->MakeShapeFromPartialTensorShape(shapes[i], &output_shape));
    c->set_output(i, output_shape);
  }
  return Status::OK();
}