static BOOL rdp_write_glyph_cache_capability_set(wStream* s, const rdpSettings* settings)
{
	size_t header;

	if (!Stream_EnsureRemainingCapacity(s, 64))
		return FALSE;

	header = rdp_capability_set_start(s);
	if (header > UINT16_MAX)
		return FALSE;
	if (settings->GlyphSupportLevel > UINT16_MAX)
		return FALSE;
	/* glyphCache (40 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[0])); /* glyphCache0 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[1])); /* glyphCache1 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[2])); /* glyphCache2 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[3])); /* glyphCache3 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[4])); /* glyphCache4 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[5])); /* glyphCache5 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[6])); /* glyphCache6 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[7])); /* glyphCache7 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[8])); /* glyphCache8 (4 bytes) */
	rdp_write_cache_definition(s, &(settings->GlyphCache[9])); /* glyphCache9 (4 bytes) */
	rdp_write_cache_definition(s, settings->FragCache);        /* fragCache (4 bytes) */
	Stream_Write_UINT16(s, (UINT16)settings->GlyphSupportLevel); /* glyphSupportLevel (2 bytes) */
	Stream_Write_UINT16(s, 0);                                 /* pad2Octets (2 bytes) */
	rdp_capability_set_finish(s, (UINT16)header, CAPSET_TYPE_GLYPH_CACHE);
	return TRUE;
}