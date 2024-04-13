static INLINE BYTE* WRITEFIRSTLINEFGBGIMAGE(BYTE* pbDest, const BYTE* pbDestEnd, BYTE bitmask,
                                            PIXEL fgPel, UINT32 cBits)
{
	BYTE mask = 0x01;

	if (cBits > 8)
		return NULL;

	if (!ENSURE_CAPACITY(pbDest, pbDestEnd, cBits))
		return NULL;

	UNROLL(cBits, {
		UINT32 data;

		if (bitmask & mask)
			data = fgPel;
		else
			data = BLACK_PIXEL;

		DESTWRITEPIXEL(pbDest, data);
		DESTNEXTPIXEL(pbDest);
		mask = mask << 1;
	});
	return pbDest;
}