BOOL glyph_cache_fragment_put(rdpGlyphCache* glyphCache, UINT32 index, UINT32 size,
                              const void* fragment)
{
	void* prevFragment;
	void* copy;

	if (index > 255)
	{
		WLog_ERR(TAG, "invalid glyph cache fragment index: %" PRIu32 "", index);
		return FALSE;
	}

	copy = malloc(size);

	if (!copy)
		return FALSE;

	WLog_Print(glyphCache->log, WLOG_DEBUG,
	           "GlyphCacheFragmentPut: index: %" PRIu32 " size: %" PRIu32 "", index, size);
	CopyMemory(copy, fragment, size);
	prevFragment = glyphCache->fragCache.entries[index].fragment;
	glyphCache->fragCache.entries[index].fragment = copy;
	glyphCache->fragCache.entries[index].size = size;
	free(prevFragment);
	return TRUE;
}