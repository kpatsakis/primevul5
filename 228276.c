point3d_filter_func(const ImageRef *ref, Image *img, const void *data, CellPixelSize cell) {
    return z_filter_func(ref, img, data, cell) && point_filter_func(ref, img, data, cell);
}