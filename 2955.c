bool MutableGraphView::RemoveControllingFaninInternal(NodeDef* node,
                                                      NodeDef* fanin_node) {
  for (int i = node->input_size() - 1; i >= 0; --i) {
    TensorId tensor_id = ParseTensorName(node->input(i));
    if (tensor_id.index() > Graph::kControlSlot) {
      break;
    }
    if (tensor_id.node() == fanin_node->name()) {
      fanouts()[{fanin_node, Graph::kControlSlot}].erase(
          {node, Graph::kControlSlot});
      node->mutable_input()->SwapElements(i, node->input_size() - 1);
      node->mutable_input()->RemoveLast();
      return true;
    }
  }
  return false;
}