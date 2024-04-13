static void rdp_write_gdiplus_cache_entries(wStream* s, UINT16 gce, UINT16 bce, UINT16 pce,
                                            UINT16 ice, UINT16 ace)
{
	Stream_Write_UINT16(s, gce); /* gdipGraphicsCacheEntries (2 bytes) */
	Stream_Write_UINT16(s, bce); /* gdipBrushCacheEntries (2 bytes) */
	Stream_Write_UINT16(s, pce); /* gdipPenCacheEntries (2 bytes) */
	Stream_Write_UINT16(s, ice); /* gdipImageCacheEntries (2 bytes) */
	Stream_Write_UINT16(s, ace); /* gdipImageAttributesCacheEntries (2 bytes) */
}