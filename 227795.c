static BOOL rdp_read_glyph_cache_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	if (length < 52)
		return FALSE;

	/* glyphCache (40 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[0])); /* glyphCache0 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[1])); /* glyphCache1 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[2])); /* glyphCache2 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[3])); /* glyphCache3 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[4])); /* glyphCache4 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[5])); /* glyphCache5 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[6])); /* glyphCache6 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[7])); /* glyphCache7 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[8])); /* glyphCache8 (4 bytes) */
	rdp_read_cache_definition(s, &(settings->GlyphCache[9])); /* glyphCache9 (4 bytes) */
	rdp_read_cache_definition(s, settings->FragCache);        /* fragCache (4 bytes) */
	Stream_Read_UINT16(s, settings->GlyphSupportLevel);       /* glyphSupportLevel (2 bytes) */
	Stream_Seek_UINT16(s);                                    /* pad2Octets (2 bytes) */
	return TRUE;
}