Status ConcatShape(InferenceContext* c, int num_inputs_to_concat) {
  return ConcatShapeHelper(c, 1 /* start_value_index */,
                           1 + num_inputs_to_concat /* end_value_index */,
                           0 /* dim_index */);
}