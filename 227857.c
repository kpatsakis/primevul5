static BOOL rdp_read_bitmap_cache_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (length < 40)
		return FALSE;

	Stream_Seek_UINT32(s); /* pad1 (4 bytes) */
	Stream_Seek_UINT32(s); /* pad2 (4 bytes) */
	Stream_Seek_UINT32(s); /* pad3 (4 bytes) */
	Stream_Seek_UINT32(s); /* pad4 (4 bytes) */
	Stream_Seek_UINT32(s); /* pad5 (4 bytes) */
	Stream_Seek_UINT32(s); /* pad6 (4 bytes) */
	Stream_Seek_UINT16(s); /* Cache0Entries (2 bytes) */
	Stream_Seek_UINT16(s); /* Cache0MaximumCellSize (2 bytes) */
	Stream_Seek_UINT16(s); /* Cache1Entries (2 bytes) */
	Stream_Seek_UINT16(s); /* Cache1MaximumCellSize (2 bytes) */
	Stream_Seek_UINT16(s); /* Cache2Entries (2 bytes) */
	Stream_Seek_UINT16(s); /* Cache2MaximumCellSize (2 bytes) */
	return TRUE;
}