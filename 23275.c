void mobi_buffer_resize(MOBIBuffer *buf, const size_t newlen) {
    unsigned char *tmp = realloc(buf->data, newlen);
    if (tmp == NULL) {
        debug_print("%s", "Buffer allocation failed\n");
        buf->error = MOBI_MALLOC_FAILED;
        return;
    }
    buf->data = tmp;
    buf->maxlen = newlen;
    if (buf->offset >= newlen) {
        buf->offset = newlen - 1;
    }
    debug_print("Buffer successfully resized to %zu\n", newlen);
    buf->error = MOBI_SUCCESS;
}