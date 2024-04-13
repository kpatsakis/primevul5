grman_resize(GraphicsManager *self, index_type UNUSED old_lines, index_type UNUSED lines, index_type UNUSED old_columns, index_type UNUSED columns) {
    self->layers_dirty = true;
}