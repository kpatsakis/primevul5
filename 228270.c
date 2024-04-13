id_filter_func(const ImageRef UNUSED *ref, Image *img, const void *data, CellPixelSize cell UNUSED) {
    uint32_t iid = *(uint32_t*)data;
    return img->client_id == iid;
}