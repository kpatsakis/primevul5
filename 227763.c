static void rdp_write_cache_definition(wStream* s, GLYPH_CACHE_DEFINITION* cache_definition)
{
	Stream_Write_UINT16(s, cache_definition->cacheEntries); /* cacheEntries (2 bytes) */
	Stream_Write_UINT16(
	    s, cache_definition->cacheMaximumCellSize); /* cacheMaximumCellSize (2 bytes) */
}