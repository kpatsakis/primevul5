static INLINE BYTE* WRITEFGBGIMAGE(BYTE* pbDest, const BYTE* pbDestEnd, UINT32 rowDelta,
                                   BYTE bitmask, PIXEL fgPel, INT32 cBits)
{
	PIXEL xorPixel;
	BYTE mask = 0x01;

	if (cBits > 8)
		return NULL;

	if (!ENSURE_CAPACITY(pbDest, pbDestEnd, cBits))
		return NULL;

	UNROLL(cBits, {
		UINT32 data;
		DESTREADPIXEL(xorPixel, pbDest - rowDelta);

		if (bitmask & mask)
			data = xorPixel ^ fgPel;
		else
			data = xorPixel;

		DESTWRITEPIXEL(pbDest, data);
		DESTNEXTPIXEL(pbDest);
		mask = mask << 1;
	});
	return pbDest;
}