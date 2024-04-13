void MutableGraphView::RemoveFaninsInternal(NodeDef* deleted_node,
                                            bool keep_controlling_fanins) {
  for (int i = 0; i < deleted_node->input_size(); ++i) {
    TensorId tensor_id = ParseTensorName(deleted_node->input(i));
    bool is_control = IsTensorIdControlling(tensor_id);
    if (keep_controlling_fanins && is_control) {
      break;
    }
    OutputPort fanin(nodes()[tensor_id.node()], tensor_id.index());

    InputPort input;
    input.node = deleted_node;
    input.port_id = is_control ? Graph::kControlSlot : i;

    auto it = fanouts().find(fanin);
    if (it != fanouts().end()) {
      absl::flat_hash_set<InputPort>* fanouts_set = &it->second;
      fanouts_set->erase(input);
      UpdateMaxRegularOutputPortForRemovedFanin(fanin, *fanouts_set);
    }
  }
  max_regular_input_port().erase(deleted_node);
}