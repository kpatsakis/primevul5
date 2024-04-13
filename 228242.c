modify_refs(GraphicsManager *self, const void* data, bool free_images, bool (*filter_func)(ImageRef*, Image*, const void*, CellPixelSize), CellPixelSize cell) {
    for (size_t i = self->image_count; i-- > 0;) {
        Image *img = self->images + i;
        for (size_t j = img->refcnt; j-- > 0;) {
            if (filter_func(img->refs + j, img, data, cell)) remove_i_from_array(img->refs, j, img->refcnt);
        }
        if (img->refcnt == 0 && (free_images || img->client_id == 0)) remove_image(self, i);
    }
}