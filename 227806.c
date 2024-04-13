static BOOL rdp_read_bitmap_cache_v2_capability_set(wStream* s, UINT16 length,
                                                    rdpSettings* settings)
{
	WINPR_UNUSED(settings);
	if (length < 40)
		return FALSE;

	Stream_Seek_UINT16(s); /* cacheFlags (2 bytes) */
	Stream_Seek_UINT8(s);  /* pad2 (1 byte) */
	Stream_Seek_UINT8(s);  /* numCellCaches (1 byte) */
	Stream_Seek(s, 4);     /* bitmapCache0CellInfo (4 bytes) */
	Stream_Seek(s, 4);     /* bitmapCache1CellInfo (4 bytes) */
	Stream_Seek(s, 4);     /* bitmapCache2CellInfo (4 bytes) */
	Stream_Seek(s, 4);     /* bitmapCache3CellInfo (4 bytes) */
	Stream_Seek(s, 4);     /* bitmapCache4CellInfo (4 bytes) */
	Stream_Seek(s, 12);    /* pad3 (12 bytes) */
	return TRUE;
}