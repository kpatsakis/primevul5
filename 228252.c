remove_image(GraphicsManager *self, size_t idx) {
    free_image(self, self->images + idx);
    remove_i_from_array(self->images, idx, self->image_count);
    self->layers_dirty = true;
}