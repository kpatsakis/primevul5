grman_alloc() {
    GraphicsManager *self = (GraphicsManager *)GraphicsManager_Type.tp_alloc(&GraphicsManager_Type, 0);
    self->images_capacity = self->capacity = 64;
    self->images = calloc(self->images_capacity, sizeof(Image));
    self->render_data = calloc(self->capacity, sizeof(ImageRenderData));
    if (self->images == NULL || self->render_data == NULL) {
        PyErr_NoMemory();
        Py_CLEAR(self); return NULL;
    }
    return self;
}