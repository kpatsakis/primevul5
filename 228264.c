ref_outside_region(const ImageRef *ref, index_type margin_top, index_type margin_bottom) {
    return ref->start_row + ref->effective_num_rows <= margin_top || ref->start_row > (int32_t)margin_bottom;
}