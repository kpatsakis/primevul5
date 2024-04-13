void mobi_buffer_free(MOBIBuffer *buf) {
	if (buf == NULL) { return; }
	if (buf->data != NULL) {
		free(buf->data);
	}
	free(buf);
}