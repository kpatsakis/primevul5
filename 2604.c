Status ConcatV2Shape(InferenceContext* c) {
  return ConcatShapeHelper(c, 0 /* start_value_index */,
                           c->num_inputs() - 1 /* end_value_index */,
                           c->num_inputs() - 1 /* dim_index */);
}