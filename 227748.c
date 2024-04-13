static void rdp_read_bitmap_cache_cell_info(wStream* s, BITMAP_CACHE_V2_CELL_INFO* cellInfo)
{
	UINT32 info;
	/**
	 * numEntries is in the first 31 bits, while the last bit (k)
	 * is used to indicate a persistent bitmap cache.
	 */
	Stream_Read_UINT32(s, info);
	cellInfo->numEntries = (info & 0x7FFFFFFF);
	cellInfo->persistent = (info & 0x80000000) ? 1 : 0;
}