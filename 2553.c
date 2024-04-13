Status UnsortedSegmentReductionShapeFn(InferenceContext* c) {
  ShapeHandle s_data = c->input(0);
  ShapeHandle s_segment_ids = c->input(1);
  ShapeHandle s_num_segments = c->input(2);
  TF_RETURN_IF_ERROR(c->WithRank(s_num_segments, 0, &s_num_segments));

  ShapeHandle out;

  // Leading dimensions of data must be compatible with dimensions of
  // <s_segment_ids>.
  if (c->RankKnown(s_segment_ids)) {
    TF_RETURN_IF_ERROR(
        c->MergePrefix(s_data, s_segment_ids, &s_data, &s_segment_ids));

    // Get the value of the num_segments input tensor.
    DimensionHandle num_segments_dim;
    TF_RETURN_IF_ERROR(c->MakeDimForScalarInput(2, &num_segments_dim));

    // Output is {segment_id_rank} + s_data[segment_id_rank:].
    ShapeHandle s_data_suffix;
    TF_RETURN_IF_ERROR(
        c->Subshape(s_data, c->Rank(s_segment_ids), &s_data_suffix));
    TF_RETURN_IF_ERROR(
        c->Concatenate(c->Vector(num_segments_dim), s_data_suffix, &out));
  } else {
    out = c->UnknownShape();
  }
  c->set_output(0, out);
  return Status::OK();
}