grman_scroll_images(GraphicsManager *self, const ScrollData *data, CellPixelSize cell) {
    if (self->image_count) {
        self->layers_dirty = true;
        modify_refs(self, data, true, data->has_margins ? scroll_filter_margins_func : scroll_filter_func, cell);
    }
}