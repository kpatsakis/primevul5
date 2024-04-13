NodeDef* MutableGraphView::GetControllingFaninToAdd(absl::string_view node_name,
                                                    const OutputPort& fanin,
                                                    string* error_msg) {
  if (!IsSwitch(*fanin.node)) {
    return fanin.node;
  } else {
    if (IsOutputPortControlling(fanin)) {
      // Can't add a Switch node control dependency.
      TensorId tensor_id(fanin.node->name(), fanin.port_id);
      *error_msg = absl::Substitute(
          "can't add fanin '$0' as it will become a Switch control dependency",
          tensor_id.ToString());
      return nullptr;
    }
    // We can't anchor control dependencies directly on the switch node: unlike
    // other nodes only one of the outputs of the switch node will be generated
    // when the switch node is executed, and we need to make sure the control
    // dependency is only triggered when the corresponding output is triggered.
    // We start by looking for an identity node connected to the output of the
    // switch node, and use it to anchor the control dependency.
    for (const auto& fanout : GetFanout(fanin)) {
      if (IsIdentity(*fanout.node) || IsIdentityNSingleInput(*fanout.node)) {
        if (fanout.node->name() == node_name) {
          *error_msg =
              absl::Substitute("can't add found fanin '$0' to self",
                               AsControlDependency(fanout.node->name()));
          return nullptr;
        }
        return fanout.node;
      }
    }

    // No node found, check if node to be created is itself.
    if (GeneratedNameForIdentityConsumingSwitch(fanin) == node_name) {
      *error_msg = absl::Substitute("can't add generated fanin '$0' to self",
                                    AsControlDependency(string(node_name)));
    }
  }
  return nullptr;
}