filter_refs(GraphicsManager *self, const void* data, bool free_images, bool (*filter_func)(const ImageRef*, Image*, const void*, CellPixelSize), CellPixelSize cell) {
    for (size_t i = self->image_count; i-- > 0;) {
        Image *img = self->images + i;
        for (size_t j = img->refcnt; j-- > 0;) {
            ImageRef *ref = img->refs + j;
            if (filter_func(ref, img, data, cell)) {
                remove_i_from_array(img->refs, j, img->refcnt);
                self->layers_dirty = true;
            }
        }
        if (img->refcnt == 0 && (free_images || img->client_id == 0)) remove_image(self, i);
    }
}