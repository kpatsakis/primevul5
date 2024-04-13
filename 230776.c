const void* glyph_cache_fragment_get(rdpGlyphCache* glyphCache, UINT32 index, UINT32* size)
{
	void* fragment;

	if (index > 255)
	{
		WLog_ERR(TAG, "invalid glyph cache fragment index: %" PRIu32 "", index);
		return NULL;
	}

	fragment = glyphCache->fragCache.entries[index].fragment;
	*size = (BYTE)glyphCache->fragCache.entries[index].size;
	WLog_Print(glyphCache->log, WLOG_DEBUG,
	           "GlyphCacheFragmentGet: index: %" PRIu32 " size: %" PRIu32 "", index, *size);

	if (!fragment)
		WLog_ERR(TAG, "invalid glyph fragment at index:%" PRIu32 "", index);

	return fragment;
}