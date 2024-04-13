free_image(GraphicsManager *self, Image *img) {
    if (img->texture_id) free_texture(&img->texture_id);
    free_refs_data(img);
    free_load_data(&(img->load_data));
    self->used_storage -= img->used_storage;
}