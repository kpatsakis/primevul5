handle_put_command(GraphicsManager *self, const GraphicsCommand *g, Cursor *c, bool *is_dirty, Image *img, CellPixelSize cell) {
    has_add_respose = false;
    if (img == NULL) img = img_by_client_id(self, g->id);
    if (img == NULL) { set_add_response("ENOENT", "Put command refers to non-existent image with id: %u", g->id); return; }
    if (!img->data_loaded) { set_add_response("ENOENT", "Put command refers to image with id: %u that could not load its data", g->id); return; }
    ensure_space_for(img, refs, ImageRef, img->refcnt + 1, refcap, 16, true);
    *is_dirty = true;
    self->layers_dirty = true;
    ImageRef *ref = NULL;
    for (size_t i=0; i < img->refcnt; i++) {
        if ((unsigned)img->refs[i].start_row == c->x && (unsigned)img->refs[i].start_column == c->y) {
            ref = img->refs + i;
            break;
        }
    }
    if (ref == NULL) {
        ref = img->refs + img->refcnt++;
        zero_at_ptr(ref);
    }
    img->atime = monotonic();
    ref->src_x = g->x_offset; ref->src_y = g->y_offset; ref->src_width = g->width ? g->width : img->width; ref->src_height = g->height ? g->height : img->height;
    ref->src_width = MIN(ref->src_width, img->width - (img->width > ref->src_x ? ref->src_x : img->width));
    ref->src_height = MIN(ref->src_height, img->height - (img->height > ref->src_y ? ref->src_y : img->height));
    ref->z_index = g->z_index;
    ref->start_row = c->y; ref->start_column = c->x;
    ref->cell_x_offset = MIN(g->cell_x_offset, cell.width - 1);
    ref->cell_y_offset = MIN(g->cell_y_offset, cell.height - 1);
    ref->num_cols = g->num_cells; ref->num_rows = g->num_lines;
    update_src_rect(ref, img);
    update_dest_rect(ref, g->num_cells, g->num_lines, cell);
    // Move the cursor, the screen will take care of ensuring it is in bounds
    c->x += ref->effective_num_cols; c->y += ref->effective_num_rows - 1;
}