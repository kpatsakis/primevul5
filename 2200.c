Status GraphProperties::AnnotateOutputShapes(GraphDef* output_graph_def) const {
  *output_graph_def = item_.graph;
  for (int i = 0; i < output_graph_def->node_size(); i++) {
    auto node = output_graph_def->mutable_node(i);
    AttrValue attr_output_shape;
    auto tensor_properties = GetOutputProperties(node->name());
    for (const auto& tensor_property : tensor_properties) {
      TensorShapeProto* proto = attr_output_shape.mutable_list()->add_shape();
      *proto = tensor_property.shape();
      NormalizeShapeForOutput(proto);
    }
    (*node->mutable_attr())["_output_shapes"] = std::move(attr_output_shape);
  }
  return Status::OK();
}