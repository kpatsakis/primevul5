void free_cache_glyph_order(rdpContext* context, CACHE_GLYPH_ORDER* glyph)
{
	if (glyph)
	{
		size_t x;

		for (x = 0; x < ARRAYSIZE(glyph->glyphData); x++)
			free(glyph->glyphData[x].aj);

		free(glyph->unicodeCharacters);
	}

	free(glyph);
}