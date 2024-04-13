NodeDef* MutableGraphView::GetOrCreateIdentityConsumingSwitch(
    const OutputPort& fanin) {
  // We haven't found an existing node where we can anchor the control
  // dependency: add a new identity node.
  string identity_name = GeneratedNameForIdentityConsumingSwitch(fanin);
  NodeDef* identity_node = GetNode(identity_name);
  if (identity_node == nullptr) {
    NodeDef new_node;
    new_node.set_name(identity_name);
    new_node.set_op("Identity");
    new_node.set_device(fanin.node->device());
    (*new_node.mutable_attr())["T"].set_type(fanin.node->attr().at("T").type());
    new_node.add_input(TensorIdToString({fanin.node->name(), fanin.port_id}));
    identity_node = AddNode(std::move(new_node));
  }
  return identity_node;
}