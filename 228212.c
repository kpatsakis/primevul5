BITMAP_INTERLEAVED_CONTEXT* bitmap_interleaved_context_new(BOOL Compressor)
{
	BITMAP_INTERLEAVED_CONTEXT* interleaved;
	interleaved = (BITMAP_INTERLEAVED_CONTEXT*)calloc(1, sizeof(BITMAP_INTERLEAVED_CONTEXT));

	if (interleaved)
	{
		interleaved->TempSize = 64 * 64 * 4;
		interleaved->TempBuffer = _aligned_malloc(interleaved->TempSize, 16);

		if (!interleaved->TempBuffer)
		{
			free(interleaved);
			WLog_ERR(TAG, "_aligned_malloc failed!");
			return NULL;
		}

		interleaved->bts = Stream_New(NULL, interleaved->TempSize);

		if (!interleaved->bts)
		{
			_aligned_free(interleaved->TempBuffer);
			free(interleaved);
			WLog_ERR(TAG, "Stream_New failed!");
			return NULL;
		}
	}

	return interleaved;
}