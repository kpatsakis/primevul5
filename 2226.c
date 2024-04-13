  Status AddFunction(const NodeDef* function_node, NameAttrList function) {
    auto it = fun_to_grappler_function_item_.find(function.name());
    if (it != fun_to_grappler_function_item_.end()) {
      return Status::OK();
    }

    const FunctionDef* function_def =
        CHECK_NOTNULL(function_library_.Find(function.name()));
    GrapplerFunctionItem grappler_function_item;
    Status function_instantiated =
        MakeGrapplerFunctionItem(*function_def, function_library_,
                                 graph_def_version_, &grappler_function_item);

    // If function instantiation failed we will skip it during shape inference.
    if (!function_instantiated.ok()) {
      VLOG(3) << "Failed to instantiate a function. Error: "
              << function_instantiated.error_message();
      fun_to_grappler_function_item_[function_def->signature().name()] =
          absl::nullopt;
      return Status::OK();
    }

    if (static_cast<int>(grappler_function_item.inputs().size()) >
        function_node->input_size()) {
      return errors::FailedPrecondition(
          "Function input size should be smaller than node input size.");
    }

    for (int i = grappler_function_item.inputs().size(),
             end = function_node->input_size();
         i < end; ++i) {
      const string& input = function_node->input(i);
      if (!IsControlInput(input)) {
        return errors::FailedPrecondition(
            "Found regular input (", input,
            ") instead of control nodes for node ", function_node->name());
      }
    }

    fun_to_grappler_function_item_[function_def->signature().name()] =
        grappler_function_item;

    return Status::OK();
  }