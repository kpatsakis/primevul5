Status MutableGraphView::UpdateAllRegularFaninsToControlling(
    absl::string_view node_name) {
  auto error_status = [node_name](absl::string_view msg) {
    string params = absl::Substitute("node_name='$0'", node_name);
    return MutationError("UpdateAllRegularFaninsToControlling", params, msg);
  };

  NodeDef* node = GetNode(node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(node_name, node, error_status));

  const int num_regular_fanins =
      NumFanins(*node, /*include_controlling_nodes=*/false);
  std::vector<OutputPort> regular_fanins;
  regular_fanins.reserve(num_regular_fanins);
  std::vector<NodeDef*> controlling_fanins;
  controlling_fanins.reserve(num_regular_fanins);

  // Get all regular fanins and derive controlling fanins.
  for (int i = 0; i < num_regular_fanins; ++i) {
    TensorId tensor_id = ParseTensorName(node->input(i));
    OutputPort fanin_port(nodes()[tensor_id.node()], tensor_id.index());

    string error_msg = "";
    NodeDef* control_node =
        GetControllingFaninToAdd(node_name, fanin_port, &error_msg);
    if (!error_msg.empty()) {
      return error_status(error_msg);
    }

    regular_fanins.push_back(fanin_port);
    controlling_fanins.push_back(control_node);
  }

  // Replace regular fanins with controlling fanins and dedup.
  int pos = 0;
  InputPort input_port(node, Graph::kControlSlot);
  absl::flat_hash_set<absl::string_view> controls;
  for (int i = 0; i < num_regular_fanins; ++i) {
    OutputPort fanin_port = regular_fanins[i];
    NodeDef* control = controlling_fanins[i];
    if (control == nullptr) {
      control = GetOrCreateIdentityConsumingSwitch(fanin_port);
    }
    fanouts()[fanin_port].erase({node, i});
    if (controls.contains(control->name())) {
      continue;
    }
    controls.insert(control->name());
    node->set_input(pos, AsControlDependency(control->name()));
    fanouts()[{control, Graph::kControlSlot}].insert(input_port);
    ++pos;
  }

  // Shift existing controlling fanins and dedup.
  for (int i = num_regular_fanins; i < node->input_size(); ++i) {
    TensorId tensor_id = ParseTensorName(node->input(i));
    if (controls.contains(tensor_id.node())) {
      continue;
    }
    controls.insert(tensor_id.node());
    node->mutable_input()->SwapElements(pos, i);
    ++pos;
  }

  // Remove duplicate controls and leftover regular fanins.
  node->mutable_input()->DeleteSubrange(pos, node->input_size() - pos);
  max_regular_input_port().erase(node);

  return Status::OK();
}