MOBIBuffer * mobi_buffer_init_null(unsigned char *data, const size_t len) {
    MOBIBuffer *buf = malloc(sizeof(MOBIBuffer));
	if (buf == NULL) {
        debug_print("%s", "Buffer allocation failed\n");
        return NULL;
    }
    buf->data = data;
	buf->offset = 0;
	buf->maxlen = len;
    buf->error = MOBI_SUCCESS;
	return buf;
}