string GeneratedNameForIdentityConsumingSwitch(
    const MutableGraphView::OutputPort& fanin) {
  return AddPrefixToNodeName(
      absl::StrCat(fanin.node->name(), "_", fanin.port_id),
      kMutableGraphViewCtrl);
}