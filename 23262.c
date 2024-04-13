void mobi_buffer_addraw(MOBIBuffer *buf, const unsigned char* data, const size_t len) {
    if (buf->offset + len > buf->maxlen) {
        debug_print("%s", "Buffer full\n");
        buf->error = MOBI_BUFFER_END;
        return;
    }
    memcpy(buf->data + buf->offset, data, len);
    buf->offset += len;
}