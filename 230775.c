CACHE_GLYPH_ORDER* copy_cache_glyph_order(rdpContext* context, const CACHE_GLYPH_ORDER* glyph)
{
	size_t x;
	CACHE_GLYPH_ORDER* dst = calloc(1, sizeof(CACHE_GLYPH_ORDER));

	if (!dst || !glyph)
		goto fail;

	*dst = *glyph;

	for (x = 0; x < glyph->cGlyphs; x++)
	{
		const GLYPH_DATA* src = &glyph->glyphData[x];
		GLYPH_DATA* data = &dst->glyphData[x];

		if (src->aj)
		{
			const size_t size = src->cb;
			data->aj = malloc(size);

			if (!data->aj)
				goto fail;

			memcpy(data->aj, src->aj, size);
		}
	}

	if (glyph->unicodeCharacters)
	{
		if (glyph->cGlyphs == 0)
			goto fail;

		dst->unicodeCharacters = calloc(glyph->cGlyphs, sizeof(WCHAR));

		if (!dst->unicodeCharacters)
			goto fail;

		memcpy(dst->unicodeCharacters, glyph->unicodeCharacters, sizeof(WCHAR) * glyph->cGlyphs);
	}

	return dst;
fail:
	free_cache_glyph_order(context, dst);
	return NULL;
}