void glyph_cache_free(rdpGlyphCache* glyphCache)
{
	if (glyphCache)
	{
		int i;
		GLYPH_CACHE* cache = glyphCache->glyphCache;

		for (i = 0; i < 10; i++)
		{
			UINT32 j;
			rdpGlyph** entries = cache[i].entries;

			if (!entries)
				continue;

			for (j = 0; j < cache[i].number; j++)
			{
				rdpGlyph* glyph = entries[j];

				if (glyph)
				{
					glyph->Free(glyphCache->context, glyph);
					entries[j] = NULL;
				}
			}

			free(entries);
			cache[i].entries = NULL;
		}

		if (glyphCache->fragCache.entries)
		{
			for (i = 0; i < 256; i++)
			{
				free(glyphCache->fragCache.entries[i].fragment);
				glyphCache->fragCache.entries[i].fragment = NULL;
			}
		}

		free(glyphCache->fragCache.entries);
		free(glyphCache);
	}
}