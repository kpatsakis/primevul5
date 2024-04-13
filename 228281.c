clear_filter_func(const ImageRef *ref, Image UNUSED *img, const void UNUSED *data, CellPixelSize cell UNUSED) {
    return ref->start_row + (int32_t)ref->effective_num_rows > 0;
}