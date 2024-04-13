void mobi_buffer_getraw(void *data, MOBIBuffer *buf, const size_t len) {
    if (!data) {
        buf->error = MOBI_PARAM_ERR;
        return;
    }
    if (buf->offset + len > buf->maxlen) {
        debug_print("%s", "End of buffer\n");
        buf->error = MOBI_BUFFER_END;
        return;
    }
    memcpy(data, buf->data + buf->offset, len);
    buf->offset += len;
}