void mobi_buffer_copy(MOBIBuffer *dest, MOBIBuffer *source, const size_t len) {
    if (source->offset + len > source->maxlen) {
        debug_print("%s", "End of buffer\n");
        source->error = MOBI_BUFFER_END;
        return;
    }
    if (dest->offset + len > dest->maxlen) {
        debug_print("%s", "End of buffer\n");
        dest->error = MOBI_BUFFER_END;
        return;
    }
    memcpy(dest->data + dest->offset, source->data + source->offset, len);
    dest->offset += len;
    source->offset += len;
}