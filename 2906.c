Status MutableGraphView::SwapNodeNames(absl::string_view from_node_name,
                                       absl::string_view to_node_name,
                                       bool update_fanouts) {
  auto error_status = [from_node_name, to_node_name,
                       update_fanouts](absl::string_view msg) {
    string params = absl::Substitute(
        "from_node_name='$0', to_node_name='$1', update_fanouts=$2",
        from_node_name, to_node_name, update_fanouts);
    return MutationError("SwapNodeNames", params, msg);
  };

  NodeDef* from_node = GetNode(from_node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(from_node_name, from_node, error_status));
  if (from_node_name == to_node_name) {
    return Status::OK();
  }
  NodeDef* to_node = GetNode(to_node_name);
  TF_RETURN_IF_ERROR(CheckNodeExists(to_node_name, to_node, error_status));

  auto swap_names = [this, from_node, to_node]() {
    nodes().erase(from_node->name());
    nodes().erase(to_node->name());
    std::swap(*from_node->mutable_name(), *to_node->mutable_name());
    nodes().emplace(from_node->name(), from_node);
    nodes().emplace(to_node->name(), to_node);
  };

  if (update_fanouts) {
    SwapFanoutInputs(*this, &fanouts(), &max_regular_output_port(), from_node,
                     to_node);
    swap_names();
    return Status::OK();
  }

  bool from_is_switch = IsSwitch(*from_node);
  MutableGraphView::OutputPort to_control(to_node, Graph::kControlSlot);
  auto to_control_fanouts = fanouts().find(to_control);
  if (from_is_switch && HasFanoutValue(fanouts(), to_control_fanouts)) {
    return error_status(SwapNodeNamesSwitchControlErrorMsg(from_node_name));
  }

  bool to_is_switch = IsSwitch(*to_node);
  MutableGraphView::OutputPort from_control(from_node, Graph::kControlSlot);
  auto from_control_fanouts = fanouts().find(from_control);
  if (to_is_switch && HasFanoutValue(fanouts(), from_control_fanouts)) {
    return error_status(SwapNodeNamesSwitchControlErrorMsg(to_node_name));
  }

  // Swap node names.
  swap_names();

  // Swap controlling fanouts.
  //
  // Note: To and from control fanout iterators are still valid as no mutations
  // has been performed on fanouts().
  SwapFanoutsMapValues(&fanouts(), from_control, from_control_fanouts,
                       to_control, to_control_fanouts);

  // Swap regular fanouts.
  SwapRegularFanoutsAndMaxPortValues(&fanouts(), &max_regular_output_port(),
                                     from_node, to_node);

  // Update fanins to remove self loops.
  auto update_fanins = [this](NodeDef* node, absl::string_view old_node_name) {
    for (int i = 0; i < node->input_size(); ++i) {
      TensorId tensor_id = ParseTensorName(node->input(i));
      if (tensor_id.node() == node->name()) {
        const int idx = tensor_id.index();
        const int node_idx =
            IsTensorIdControlling(tensor_id) ? Graph::kControlSlot : i;

        MutableGraphView::OutputPort from_fanin(node, idx);
        absl::flat_hash_set<InputPort>* from_fanouts = &fanouts()[from_fanin];
        from_fanouts->erase({node, node_idx});
        UpdateMaxRegularOutputPortForRemovedFanin(from_fanin, *from_fanouts);

        MutableGraphView::OutputPort to_fanin(nodes().at(old_node_name), idx);
        fanouts()[to_fanin].insert({node, node_idx});
        UpdateMaxRegularOutputPortForAddedFanin(to_fanin);
        node->set_input(i, TensorIdToString({old_node_name, idx}));
      }
    }
  };
  update_fanins(from_node, to_node->name());
  update_fanins(to_node, from_node->name());

  // Dedup control dependencies.
  auto dedup_control_fanouts =
      [this](NodeDef* node, const FanoutsMap::iterator& control_fanouts) {
        if (CanDedupControlWithRegularInput(*this, *node) &&
            control_fanouts != fanouts().end()) {
          for (auto it = control_fanouts->second.begin();
               it != control_fanouts->second.end();) {
            // Advance `it` before invalidation from removal.
            const auto& control_fanout = *it++;
            if (HasRegularFaninNode(*this, *control_fanout.node,
                                    node->name())) {
              RemoveControllingFaninInternal(control_fanout.node, node);
            }
          }
        }
      };
  auto dedup_switch_control = [this, dedup_control_fanouts](NodeDef* node) {
    OutputPort port;
    port.node = node;
    const int max_port =
        gtl::FindWithDefault(max_regular_output_port(), node, -1);
    for (int i = 0; i <= max_port; ++i) {
      port.port_id = i;
      auto it = fanouts().find(port);
      if (it == fanouts().end()) {
        continue;
      }
      for (const auto& fanout : it->second) {
        auto fanout_controls =
            fanouts().find({fanout.node, Graph::kControlSlot});
        dedup_control_fanouts(fanout.node, fanout_controls);
      }
    }
  };

  if (!from_is_switch) {
    if (to_is_switch) {
      dedup_switch_control(from_node);
    } else {
      // Fetch iterator again as the original iterator might have been
      // invalidated by container rehash triggered due to mutations.
      auto from_control_fanouts = fanouts().find(from_control);
      dedup_control_fanouts(from_node, from_control_fanouts);
    }
  }
  if (!to_is_switch) {
    if (from_is_switch) {
      dedup_switch_control(to_node);
    } else {
      // Fetch iterator again as the original iterator might have been
      // invalidated by container rehash triggered due to mutations.
      auto to_control_fanouts = fanouts().find(to_control);
      dedup_control_fanouts(to_node, to_control_fanouts);
    }
  }

  return Status::OK();
}