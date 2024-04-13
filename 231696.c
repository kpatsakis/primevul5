static INLINE BOOL updateContextFormat(CLEAR_CONTEXT* clear, UINT32 DstFormat)
{
	if (!clear || !clear->nsc)
		return FALSE;

	clear->format = DstFormat;
	return nsc_context_set_pixel_format(clear->nsc, DstFormat);
}