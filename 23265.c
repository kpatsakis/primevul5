MOBIBuffer * mobi_buffer_init(const size_t len) {
    unsigned char *data = malloc(len);
    if (data == NULL) {
        debug_print("%s", "Buffer data allocation failed\n");
        return NULL;
    }
    MOBIBuffer *buf = mobi_buffer_init_null(data, len);
    if (buf == NULL) {
        free(data);
    }
    return buf;
}