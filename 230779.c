static BOOL update_gdi_fast_index(rdpContext* context, const FAST_INDEX_ORDER* fastIndex)
{
	INT32 x, y;
	INT32 opLeft, opTop;
	INT32 opRight, opBottom;
	INT32 opWidth = 0, opHeight = 0;
	INT32 bkWidth = 0, bkHeight = 0;

	if (!context || !fastIndex || !context->cache)
		return FALSE;

	opLeft = fastIndex->opLeft;
	opTop = fastIndex->opTop;
	opRight = fastIndex->opRight;
	opBottom = fastIndex->opBottom;
	x = fastIndex->x;
	y = fastIndex->y;

	if (opBottom == -32768)
	{
		BYTE flags = (BYTE)(opTop & 0x0F);

		if (flags & 0x01)
			opBottom = fastIndex->bkBottom;

		if (flags & 0x02)
			opRight = fastIndex->bkRight;

		if (flags & 0x04)
			opTop = fastIndex->bkTop;

		if (flags & 0x08)
			opLeft = fastIndex->bkLeft;
	}

	if (opLeft == 0)
		opLeft = fastIndex->bkLeft;

	if (opRight == 0)
		opRight = fastIndex->bkRight;

	/* Server can send a massive number (32766) which appears to be
	 * undocumented special behavior for "Erase all the way right".
	 * X11 has nondeterministic results asking for a draw that wide. */
	if (opRight > (INT64)context->instance->settings->DesktopWidth)
		opRight = (int)context->instance->settings->DesktopWidth;

	if (x == -32768)
		x = fastIndex->bkLeft;

	if (y == -32768)
		y = fastIndex->bkTop;

	if (fastIndex->bkRight > fastIndex->bkLeft)
		bkWidth = fastIndex->bkRight - fastIndex->bkLeft + 1;

	if (fastIndex->bkBottom > fastIndex->bkTop)
		bkHeight = fastIndex->bkBottom - fastIndex->bkTop + 1;

	if (opRight > opLeft)
		opWidth = opRight - opLeft + 1;

	if (opBottom > opTop)
		opHeight = opBottom - opTop + 1;

	return update_process_glyph_fragments(
	    context, fastIndex->data, fastIndex->cbData, fastIndex->cacheId, fastIndex->ulCharInc,
	    fastIndex->flAccel, fastIndex->backColor, fastIndex->foreColor, x, y, fastIndex->bkLeft,
	    fastIndex->bkTop, bkWidth, bkHeight, opLeft, opTop, opWidth, opHeight, FALSE);
}