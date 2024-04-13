static void rdp_write_bitmap_cache_cell_info(wStream* s, BITMAP_CACHE_V2_CELL_INFO* cellInfo)
{
	UINT32 info;
	/**
	 * numEntries is in the first 31 bits, while the last bit (k)
	 * is used to indicate a persistent bitmap cache.
	 */
	info = (cellInfo->numEntries | (cellInfo->persistent << 31));
	Stream_Write_UINT32(s, info);
}