static BOOL resize_vbar_entry(CLEAR_CONTEXT* clear, CLEAR_VBAR_ENTRY* vBarEntry)
{
	if (vBarEntry->count > vBarEntry->size)
	{
		const UINT32 bpp = GetBytesPerPixel(clear->format);
		const UINT32 oldPos = vBarEntry->size * bpp;
		const UINT32 diffSize = (vBarEntry->count - vBarEntry->size) * bpp;
		BYTE* tmp;
		vBarEntry->size = vBarEntry->count;
		tmp = (BYTE*)realloc(vBarEntry->pixels, vBarEntry->count * bpp);

		if (!tmp)
		{
			WLog_ERR(TAG, "vBarEntry->pixels realloc %" PRIu32 " failed", vBarEntry->count * bpp);
			return FALSE;
		}

		memset(&tmp[oldPos], 0, diffSize);
		vBarEntry->pixels = tmp;
	}

	if (!vBarEntry->pixels && vBarEntry->size)
	{
		WLog_ERR(TAG, "vBarEntry->pixels is NULL but vBarEntry->size is %" PRIu32 "",
		         vBarEntry->size);
		return FALSE;
	}

	return TRUE;
}