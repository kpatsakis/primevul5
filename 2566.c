Status UnchangedShape(shape_inference::InferenceContext* c) {
  c->set_output(0, c->input(0));
  auto* handle_data = c->input_handle_shapes_and_types(0);
  if (handle_data != nullptr) {
    c->set_output_handle_shapes_and_types(0, *handle_data);
  }
  return Status::OK();
}