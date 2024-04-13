void mobi_buffer_free_null(MOBIBuffer *buf) {
	if (buf == NULL) { return; }
	free(buf);
}