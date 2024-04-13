find_or_create_image(GraphicsManager *self, uint32_t id, bool *existing) {
    if (id) {
        for (size_t i = 0; i < self->image_count; i++) {
            if (self->images[i].client_id == id) {
                *existing = true;
                return self->images + i;
            }
        }
    }
    *existing = false;
    ensure_space_for(self, images, Image, self->image_count + 1, images_capacity, 64, true);
    Image *ans = self->images + self->image_count++;
    zero_at_ptr(ans);
    return ans;
}