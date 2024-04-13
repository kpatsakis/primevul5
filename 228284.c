W(update_layers) {
    unsigned int scrolled_by, sx, sy; float xstart, ystart, dx, dy;
    CellPixelSize cell;
    PA("IffffIIII", &scrolled_by, &xstart, &ystart, &dx, &dy, &sx, &sy, &cell.width, &cell.height);
    grman_update_layers(self, scrolled_by, xstart, ystart, dx, dy, sx, sy, cell);
    PyObject *ans = PyTuple_New(self->count);
    for (size_t i = 0; i < self->count; i++) {
        ImageRenderData *r = self->render_data + i;
#define R(offset) Py_BuildValue("{sf sf sf sf}", "left", r->vertices[offset + 8], "top", r->vertices[offset + 1], "right", r->vertices[offset], "bottom", r->vertices[offset + 5])
        PyTuple_SET_ITEM(ans, i,
            Py_BuildValue("{sN sN sI si sK}", "src_rect", R(0), "dest_rect", R(2), "group_count", r->group_count, "z_index", r->z_index, "image_id", r->image_id)
        );
#undef R
    }
    return ans;
}