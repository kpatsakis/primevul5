z_filter_func(const ImageRef *ref, Image UNUSED *img, const void *data, CellPixelSize cell UNUSED) {
    const GraphicsCommand *g = data;
    return ref->z_index == g->z_index;
}