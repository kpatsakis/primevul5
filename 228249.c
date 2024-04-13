remove_images(GraphicsManager *self, bool(*predicate)(Image*), id_type skip_image_internal_id) {
    for (size_t i = self->image_count; i-- > 0;) {
        Image *img = self->images + i;
        if (img->internal_id != skip_image_internal_id && predicate(img)) {
            remove_image(self, i);
        }
    }
}