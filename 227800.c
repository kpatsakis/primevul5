static BOOL rdp_print_glyph_cache_capability_set(wStream* s, UINT16 length)
{
	GLYPH_CACHE_DEFINITION glyphCache[10];
	GLYPH_CACHE_DEFINITION fragCache;
	UINT16 glyphSupportLevel;
	UINT16 pad2Octets;
	WLog_INFO(TAG, "GlyphCacheCapabilitySet (length %" PRIu16 "):", length);

	if (length < 52)
		return FALSE;

	/* glyphCache (40 bytes) */
	rdp_read_cache_definition(s, &glyphCache[0]); /* glyphCache0 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[1]); /* glyphCache1 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[2]); /* glyphCache2 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[3]); /* glyphCache3 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[4]); /* glyphCache4 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[5]); /* glyphCache5 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[6]); /* glyphCache6 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[7]); /* glyphCache7 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[8]); /* glyphCache8 (4 bytes) */
	rdp_read_cache_definition(s, &glyphCache[9]); /* glyphCache9 (4 bytes) */
	rdp_read_cache_definition(s, &fragCache);     /* fragCache (4 bytes) */
	Stream_Read_UINT16(s, glyphSupportLevel);     /* glyphSupportLevel (2 bytes) */
	Stream_Read_UINT16(s, pad2Octets);            /* pad2Octets (2 bytes) */
	WLog_INFO(TAG, "\tglyphCache0: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[0].cacheEntries, glyphCache[0].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache1: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[1].cacheEntries, glyphCache[1].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache2: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[2].cacheEntries, glyphCache[2].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache3: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[3].cacheEntries, glyphCache[3].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache4: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[4].cacheEntries, glyphCache[4].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache5: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[5].cacheEntries, glyphCache[5].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache6: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[6].cacheEntries, glyphCache[6].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache7: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[7].cacheEntries, glyphCache[7].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache8: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[8].cacheEntries, glyphCache[8].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphCache9: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          glyphCache[9].cacheEntries, glyphCache[9].cacheMaximumCellSize);
	WLog_INFO(TAG, "\tfragCache: Entries: %" PRIu16 " MaximumCellSize: %" PRIu16 "",
	          fragCache.cacheEntries, fragCache.cacheMaximumCellSize);
	WLog_INFO(TAG, "\tglyphSupportLevel: 0x%04" PRIX16 "", glyphSupportLevel);
	WLog_INFO(TAG, "\tpad2Octets: 0x%04" PRIX16 "", pad2Octets);
	return TRUE;
}