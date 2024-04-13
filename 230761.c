static BOOL update_gdi_glyph_index(rdpContext* context, GLYPH_INDEX_ORDER* glyphIndex)
{
	INT32 bkWidth = 0, bkHeight = 0, opWidth = 0, opHeight = 0;

	if (!context || !glyphIndex || !context->cache)
		return FALSE;

	if (glyphIndex->bkRight > glyphIndex->bkLeft)
		bkWidth = glyphIndex->bkRight - glyphIndex->bkLeft + 1;

	if (glyphIndex->opRight > glyphIndex->opLeft)
		opWidth = glyphIndex->opRight - glyphIndex->opLeft + 1;

	if (glyphIndex->bkBottom > glyphIndex->bkTop)
		bkHeight = glyphIndex->bkBottom - glyphIndex->bkTop + 1;

	if (glyphIndex->opBottom > glyphIndex->opTop)
		opHeight = glyphIndex->opBottom - glyphIndex->opTop + 1;

	return update_process_glyph_fragments(
	    context, glyphIndex->data, glyphIndex->cbData, glyphIndex->cacheId, glyphIndex->ulCharInc,
	    glyphIndex->flAccel, glyphIndex->backColor, glyphIndex->foreColor, glyphIndex->x,
	    glyphIndex->y, glyphIndex->bkLeft, glyphIndex->bkTop, bkWidth, bkHeight, glyphIndex->opLeft,
	    glyphIndex->opTop, opWidth, opHeight, glyphIndex->fOpRedundant);
}