unsigned char * mobi_buffer_getpointer(MOBIBuffer *buf, const size_t len) {
    if (buf->offset + len > buf->maxlen) {
        debug_print("%s", "End of buffer\n");
        buf->error = MOBI_BUFFER_END;
        return NULL;
    }
    buf->offset += len;
    return buf->data + buf->offset - len;
}