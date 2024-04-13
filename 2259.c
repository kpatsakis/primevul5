void VerboseLogUnknownDimensionSources(
    const GraphDef& graph,
    const absl::flat_hash_map<string, std::vector<OpInfo::TensorProperties>>&
        input_properties_map,
    const absl::flat_hash_map<string, std::vector<OpInfo::TensorProperties>>&
        output_properties_map) {
  if (!VLOG_IS_ON(2)) {
    return;
  }

  VLOG(2) << "Nodes with known inputs, but with unknown output dimensions:";

  // Find all nodes in the graph for which we
  // do not have any unknown dimensions in their inputs, but
  // we have some unknown dimensions in their outputs.
  std::map<string, int> op_to_count;
  for (const NodeDef& node : graph.node()) {
    const auto& input_properties = input_properties_map.at(node.name());
    const auto& output_properties = output_properties_map.at(node.name());

    bool has_unknown_inputs = false;
    for (const auto& input_prop : input_properties) {
      if (HasAnyUnknownDimensions(input_prop.shape())) {
        has_unknown_inputs = true;
        break;
      }
    }

    if (has_unknown_inputs) {
      continue;
    }

    for (const auto& output_prop : output_properties) {
      if (HasAnyUnknownDimensions(output_prop.shape())) {
        string inputs = "input_shapes=[";
        for (const auto& input_prop : input_properties) {
          inputs += PartialTensorShape::DebugString(input_prop.shape());
        }
        inputs += "]";

        string outputs = "output_shapes=[";
        for (const auto& output_prop : output_properties) {
          outputs += PartialTensorShape::DebugString(output_prop.shape());
        }
        outputs += "]";

        VLOG(2) << "Node: " << node.name() << ", Op: " << node.op() << ", "
                << inputs << ", " << outputs;

        op_to_count[node.op()]++;

        // don't log again for this node
        break;
      }
    }
  }
  VLOG(2) << "Op types with known inputs, but with unknown output dimensions "
          << "(format: <op_type> (<count>)):";
  for (const auto& p : op_to_count) {
    VLOG(2) << p.first << " (" << p.second << ")";
  }
}