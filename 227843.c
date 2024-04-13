static BOOL rdp_write_bitmap_cache_host_support_capability_set(wStream* s,
                                                               const rdpSettings* settings)
{
	size_t header;

	WINPR_UNUSED(settings);
	if (!Stream_EnsureRemainingCapacity(s, 32))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	Stream_Write_UINT8(s, BITMAP_CACHE_V2); /* cacheVersion (1 byte) */
	Stream_Write_UINT8(s, 0);               /* pad1 (1 byte) */
	Stream_Write_UINT16(s, 0);              /* pad2 (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_BITMAP_CACHE_HOST_SUPPORT);
	return TRUE;
}