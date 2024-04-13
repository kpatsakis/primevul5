static BOOL rdp_write_large_pointer_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT16 largePointerSupportFlags;

	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	largePointerSupportFlags =
	    settings->LargePointerFlag & (LARGE_POINTER_FLAG_96x96 | LARGE_POINTER_FLAG_384x384);
	Stream_Write_UINT16(s, largePointerSupportFlags); /* largePointerSupportFlags (2 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_LARGE_POINTER);
	return TRUE;
}