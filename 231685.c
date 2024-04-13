CLEAR_CONTEXT* clear_context_new(BOOL Compressor)
{
	CLEAR_CONTEXT* clear;
	clear = (CLEAR_CONTEXT*)calloc(1, sizeof(CLEAR_CONTEXT));

	if (!clear)
		return NULL;

	clear->Compressor = Compressor;
	clear->nsc = nsc_context_new();

	if (!clear->nsc)
		goto error_nsc;

	if (!updateContextFormat(clear, PIXEL_FORMAT_BGRX32))
		goto error_nsc;

	if (!clear_resize_buffer(clear, 512, 512))
		goto error_nsc;

	if (!clear->TempBuffer)
		goto error_nsc;

	if (!clear_context_reset(clear))
		goto error_nsc;

	return clear;
error_nsc:
	clear_context_free(clear);
	return NULL;
}