rdpGlyph* glyph_cache_get(rdpGlyphCache* glyphCache, UINT32 id, UINT32 index)
{
	rdpGlyph* glyph;
	WLog_Print(glyphCache->log, WLOG_DEBUG, "GlyphCacheGet: id: %" PRIu32 " index: %" PRIu32 "", id,
	           index);

	if (id > 9)
	{
		WLog_ERR(TAG, "invalid glyph cache id: %" PRIu32 "", id);
		return NULL;
	}

	if (index > glyphCache->glyphCache[id].number)
	{
		WLog_ERR(TAG, "index %" PRIu32 " out of range for cache id: %" PRIu32 "", index, id);
		return NULL;
	}

	glyph = glyphCache->glyphCache[id].entries[index];

	if (!glyph)
		WLog_ERR(TAG, "no glyph found at cache index: %" PRIu32 " in cache id: %" PRIu32 "", index,
		         id);

	return glyph;
}