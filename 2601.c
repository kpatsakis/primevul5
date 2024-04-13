Status EinsumShape(shape_inference::InferenceContext* c) {
  // We assume that the equation has a valid format. Either (x),(y)->(z)
  // or (x)->(z), where each of (x), (y) and (z) are concatenation of zero or
  // more latin alphabets and contains at most one ellipsis ('...').
  string equation;
  TF_RETURN_IF_ERROR(c->GetAttr("equation", &equation));
  gtl::InlinedVector<string, 2> input_labels;
  string output_labels;
  TF_RETURN_IF_ERROR(
      ParseEinsumEquation(equation, &input_labels, &output_labels));

  if (c->num_inputs() == 0 || c->num_inputs() > 2) {
    return errors::InvalidArgument("Expected either 1 or 2 inputs but got: ",
                                   c->num_inputs());
  }
  const int input_labels_size = input_labels.size();
  if (c->num_inputs() != input_labels_size) {
    return errors::InvalidArgument("Expected ", input_labels.size(),
                                   " inputs for equation ", equation,
                                   " but got: ", c->num_inputs());
  }

  // Validate input subscripts, build the label to dimension mapping and obtain
  // the broadcast shapes that map to ellipsis.
  absl::flat_hash_map<char, DimensionHandle> label_to_dimension;
  gtl::InlinedVector<ShapeHandle, 2> input_bcast_shapes(c->num_inputs());
  for (int i = 0, end = c->num_inputs(); i < end; ++i) {
    bool has_ellipsis = false;
    TF_RETURN_IF_ERROR(ValidateEinsumEllipsis(input_labels[i], &has_ellipsis));
    ShapeHandle input_shape = c->input(i);
    // Validate that the input rank is sufficient for the given number of named
    // labels.
    if (c->RankKnown(input_shape)) {
      if (has_ellipsis) {
        const int num_named_labels =
            static_cast<int>(input_labels[i].size()) - 3;
        TF_RETURN_WITH_CONTEXT_IF_ERROR(
            c->WithRankAtLeast(input_shape, num_named_labels, &input_shape),
            " for ", i, "th input and equation: ", equation);
      } else {
        const int num_named_labels = static_cast<int>(input_labels[i].size());
        TF_RETURN_WITH_CONTEXT_IF_ERROR(
            c->WithRank(input_shape, num_named_labels, &input_shape), " for ",
            i, "th input and equation: ", equation);
      }
    }

    bool seen_ellipsis = false;
    input_bcast_shapes[i] = c->Scalar();
    // Run through the input labels; populate label_to_dimension mapping and
    // compute the broadcast shapes corresponding to the ellipsis (if present).
    for (int label_idx = 0, end = input_labels[i].size(); label_idx < end;
         ++label_idx) {
      const char label = input_labels[i][label_idx];
      // Calculate the input axis that the current label is referring to. After
      // the ellipsis, the axis may be found by using negative indices; i.e the
      // (rank - k)th dimension corresponds to the (num_labels - k)th label.
      const int64_t axis_before_ellipsis = label_idx;
      const int64_t axis_after_ellipsis =
          c->RankKnown(input_shape)
              ? label_idx + c->Rank(input_shape) - input_labels[i].size()
              : -1;

      // Populate the input broadcast shape when we encounter an ellipsis (...).
      if (label == '.') {
        if (!c->RankKnown(input_shape)) {
          input_bcast_shapes[i] = c->UnknownShape();
        } else {
          // The broadcast shape runs till the named label right after the
          // ellipsis, the label with index (label_idx + 3).
          TF_RETURN_IF_ERROR(c->Subshape(input_shape, axis_before_ellipsis,
                                         axis_after_ellipsis + 3,
                                         &input_bcast_shapes[i]));
        }
        label_idx += 2;  // Skip the rest of the ellipsis.
        seen_ellipsis = true;
        continue;
      }
      // Obtain the dimension that the current label corresponds to.
      int64_t axis = seen_ellipsis ? axis_after_ellipsis : axis_before_ellipsis;
      DimensionHandle new_dim = c->RankKnown(input_shape)
                                    ? c->Dim(input_shape, axis)
                                    : c->UnknownDim();
      // If we've seen this label before, make sure previous and current
      // dimensions are compatible.
      if (label_to_dimension.contains(label)) {
        DimensionHandle merged;
        TF_RETURN_IF_ERROR(
            c->Merge(label_to_dimension[label], new_dim, &merged));
        label_to_dimension[label] = merged;
      } else {
        label_to_dimension[label] = new_dim;
      }
    }
  }

  // For two inputs, broadcast the two input broadcast shapes to create the
  // output broadcast shape. For one input, just copy the single broadcast
  // shape.
  ShapeHandle output_bcast_shape;
  if (input_bcast_shapes.size() == 1) {
    output_bcast_shape = input_bcast_shapes[0];
  } else if (input_bcast_shapes.size() == 2) {
    TF_RETURN_IF_ERROR(BroadcastBinaryOpOutputShapeFnHelper(
        c, input_bcast_shapes[0], input_bcast_shapes[1], true,
        &output_bcast_shape));
  }

  bool output_has_ellipsis = false;
  TF_RETURN_IF_ERROR(
      ValidateEinsumEllipsis(output_labels, &output_has_ellipsis));
  if (output_has_ellipsis) {
    // If the output subscript has ellipsis and the output broadcast rank is
    // unknown, then the output shape should have unknown rank.
    if (!c->RankKnown(output_bcast_shape)) {
      c->set_output(0, c->UnknownShape());
      return Status::OK();
    }
  } else {
    // If the output subscripts don't have ellipsis then make sure the output
    // broadcasting shape is empty.
    TF_RETURN_WITH_CONTEXT_IF_ERROR(
        c->WithRankAtMost(output_bcast_shape, 0, &output_bcast_shape),
        " for einsum equation '", equation,
        "' without ellipsis (...) in the output subscripts where input(s) have "
        "non-empty broadcasting shape");
    output_bcast_shape = c->Scalar();
  }

  // Create the output shape from output labels and label_to_dimension mapping.
  std::vector<DimensionHandle> output_dims;
  for (int label_idx = 0, end = output_labels.size(); label_idx < end;
       ++label_idx) {
    const char label = output_labels[label_idx];
    // Append the output_bcast_shape when the ellipsis is encountered.
    if (label == '.') {
      for (int k = 0; k < c->Rank(output_bcast_shape); ++k) {
        output_dims.push_back(c->Dim(output_bcast_shape, k));
      }
      label_idx += 2;  // Skip the rest of the ellipsis.
      continue;
    }
    auto dimension_it = label_to_dimension.find(label);
    if (dimension_it == label_to_dimension.end()) {
      return errors::InvalidArgument(
          "Einsum output subscripts for equation '", equation, "' has label '",
          label, "' which is not present in the input subscripts");
    }
    output_dims.push_back(dimension_it->second);
  }
  c->set_output(0, c->MakeShape(output_dims));
  return Status::OK();
}