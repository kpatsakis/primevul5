static BOOL clear_resize_buffer(CLEAR_CONTEXT* clear, UINT32 width, UINT32 height)
{
	UINT32 size;

	if (!clear)
		return FALSE;

	size = ((width + 16) * (height + 16) * GetBytesPerPixel(clear->format));

	if (size > clear->TempSize)
	{
		BYTE* tmp = (BYTE*)realloc(clear->TempBuffer, size);

		if (!tmp)
		{
			WLog_ERR(TAG, "clear->TempBuffer realloc failed for %" PRIu32 " bytes", size);
			return FALSE;
		}

		clear->TempSize = size;
		clear->TempBuffer = tmp;
	}

	return TRUE;
}