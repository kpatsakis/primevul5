static BOOL rdp_write_bitmap_cache_v2_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;
	UINT16 cacheFlags;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	header = rdp_capability_set_start(s);
	cacheFlags = ALLOW_CACHE_WAITING_LIST_FLAG;

	if (settings->BitmapCachePersistEnabled)
		cacheFlags |= PERSISTENT_KEYS_EXPECTED_FLAG;

	Stream_Write_UINT16(s, cacheFlags);                     /* cacheFlags (2 bytes) */
	Stream_Write_UINT8(s, 0);                               /* pad2 (1 byte) */
	Stream_Write_UINT8(s, settings->BitmapCacheV2NumCells); /* numCellCaches (1 byte) */
	rdp_write_bitmap_cache_cell_info(
	    s, &settings->BitmapCacheV2CellInfo[0]); /* bitmapCache0CellInfo (4 bytes) */
	rdp_write_bitmap_cache_cell_info(
	    s, &settings->BitmapCacheV2CellInfo[1]); /* bitmapCache1CellInfo (4 bytes) */
	rdp_write_bitmap_cache_cell_info(
	    s, &settings->BitmapCacheV2CellInfo[2]); /* bitmapCache2CellInfo (4 bytes) */
	rdp_write_bitmap_cache_cell_info(
	    s, &settings->BitmapCacheV2CellInfo[3]); /* bitmapCache3CellInfo (4 bytes) */
	rdp_write_bitmap_cache_cell_info(
	    s, &settings->BitmapCacheV2CellInfo[4]); /* bitmapCache4CellInfo (4 bytes) */
	Stream_Zero(s, 12);                          /* pad3 (12 bytes) */
	rdp_capability_set_finish(s, header, CAPSET_TYPE_BITMAP_CACHE_V2);
	return TRUE;
}