grman_handle_command(GraphicsManager *self, const GraphicsCommand *g, const uint8_t *payload, Cursor *c, bool *is_dirty, CellPixelSize cell) {
    const char *ret = NULL;

    switch(g->action) {
        case 0:
        case 't':
        case 'T':
        case 'q': {
            uint32_t iid = g->id, q_iid = iid;
            if (g->action == 'q') { iid = 0; if (!q_iid) { REPORT_ERROR("Query graphics command without image id"); break; } }
            Image *image = handle_add_command(self, g, payload, is_dirty, iid);
            ret = create_add_response(self, image != NULL, g->action == 'q' ? q_iid: self->last_init_graphics_command.id);
            if (self->last_init_graphics_command.action == 'T' && image && image->data_loaded) handle_put_command(self, &self->last_init_graphics_command, c, is_dirty, image, cell);
            id_type added_image_id = image ? image->internal_id : 0;
            if (g->action == 'q') remove_images(self, add_trim_predicate, 0);
            if (self->used_storage > STORAGE_LIMIT) apply_storage_quota(self, STORAGE_LIMIT, added_image_id);
            break;
        }
        case 'p':
            if (!g->id) {
                REPORT_ERROR("Put graphics command without image id");
                break;
            }
            handle_put_command(self, g, c, is_dirty, NULL, cell);
            ret = create_add_response(self, true, g->id);
            break;
        case 'd':
            handle_delete_command(self, g, c, is_dirty, cell);
            break;
        default:
            REPORT_ERROR("Unknown graphics command action: %c", g->action);
            break;
    }
    return ret;
}