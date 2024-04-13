point_filter_func(const ImageRef *ref, Image *img, const void *data, CellPixelSize cell) {
    return x_filter_func(ref, img, data, cell) && y_filter_func(ref, img, data, cell);
}