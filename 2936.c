void MutableGraphView::RemoveFanoutsInternal(NodeDef* deleted_node) {
  const int max_port =
      gtl::FindWithDefault(max_regular_output_port(), deleted_node, -1);
  for (int i = Graph::kControlSlot; i <= max_port; ++i) {
    fanouts().erase({deleted_node, i});
  }
  max_regular_output_port().erase(deleted_node);
}