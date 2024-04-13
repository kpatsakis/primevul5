grman_update_layers(GraphicsManager *self, unsigned int scrolled_by, float screen_left, float screen_top, float dx, float dy, unsigned int num_cols, unsigned int num_rows, CellPixelSize cell) {
    if (self->last_scrolled_by != scrolled_by) self->layers_dirty = true;
    self->last_scrolled_by = scrolled_by;
    if (!self->layers_dirty) return false;
    self->layers_dirty = false;
    size_t i, j;
    self->num_of_below_refs = 0;
    self->num_of_negative_refs = 0;
    self->num_of_positive_refs = 0;
    Image *img; ImageRef *ref;
    ImageRect r;
    float screen_width = dx * num_cols, screen_height = dy * num_rows;
    float screen_bottom = screen_top - screen_height;
    float screen_width_px = num_cols * cell.width;
    float screen_height_px = num_rows * cell.height;
    float y0 = screen_top - dy * scrolled_by;

    // Iterate over all visible refs and create render data
    self->count = 0;
    for (i = 0; i < self->image_count; i++) { img = self->images + i; for (j = 0; j < img->refcnt; j++) { ref = img->refs + j;
        r.top = y0 - ref->start_row * dy - dy * (float)ref->cell_y_offset / (float)cell.height;
        if (ref->num_rows > 0) r.bottom = y0 - (ref->start_row + (int32_t)ref->num_rows) * dy;
        else r.bottom = r.top - screen_height * (float)ref->src_height / screen_height_px;
        if (r.top <= screen_bottom || r.bottom >= screen_top) continue;  // not visible

        r.left = screen_left + ref->start_column * dx + dx * (float)ref->cell_x_offset / (float) cell.width;
        if (ref->num_cols > 0) r.right = screen_left + (ref->start_column + (int32_t)ref->num_cols) * dx;
        else r.right = r.left + screen_width * (float)ref->src_width / screen_width_px;

        if (ref->z_index < ((int32_t)INT32_MIN/2))
            self->num_of_below_refs++;
        else if (ref->z_index < 0)
            self->num_of_negative_refs++;
        else
            self->num_of_positive_refs++;
        ensure_space_for(self, render_data, ImageRenderData, self->count + 1, capacity, 64, true);
        ImageRenderData *rd = self->render_data + self->count;
        zero_at_ptr(rd);
        set_vertex_data(rd, ref, &r);
        self->count++;
        rd->z_index = ref->z_index; rd->image_id = img->internal_id;
        rd->texture_id = img->texture_id;
    }}
    if (!self->count) return false;
    // Sort visible refs in draw order (z-index, img)
    qsort(self->render_data, self->count, sizeof(self->render_data[0]), cmp_by_zindex_and_image);
    // Calculate the group counts
    i = 0;
    while (i < self->count) {
        id_type image_id = self->render_data[i].image_id, start = i;
        if (start == self->count - 1) i = self->count;
        else {
            while (i < self->count - 1 && self->render_data[++i].image_id == image_id) {}
        }
        self->render_data[start].group_count = i - start;
    }
    return true;
}