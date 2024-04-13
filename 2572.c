Status QuantizedConcatV2Shape(InferenceContext* c, int num_inputs_to_concat) {
  return ConcatShapeHelper(c, 0 /* start_value_index */,
                           num_inputs_to_concat /* end_value_index */,
                           num_inputs_to_concat /* dim_index */);
}