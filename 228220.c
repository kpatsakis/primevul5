void bitmap_interleaved_context_free(BITMAP_INTERLEAVED_CONTEXT* interleaved)
{
	if (!interleaved)
		return;

	_aligned_free(interleaved->TempBuffer);
	Stream_Free(interleaved->bts, TRUE);
	free(interleaved);
}