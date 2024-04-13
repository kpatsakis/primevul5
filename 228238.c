create_add_response(GraphicsManager UNUSED *self, bool data_loaded, uint32_t iid) {
    static char rbuf[sizeof(add_response)/sizeof(add_response[0]) + 64];
    if (iid) {
        if (!has_add_respose) {
            if (!data_loaded) return NULL;
            snprintf(add_response, 10, "OK");
        }
        snprintf(rbuf, sizeof(rbuf)/sizeof(rbuf[0]) - 1, "Gi=%u;%s", iid, add_response);
        return rbuf;
    }
    return NULL;
}