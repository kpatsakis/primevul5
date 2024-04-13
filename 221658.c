static BOOL _update_read_pointer_large(wStream* s, POINTER_LARGE_UPDATE* pointer)
{
	BYTE* newMask;
	UINT32 scanlineSize;

	if (!pointer)
		goto fail;

	if (Stream_GetRemainingLength(s) < 14)
		goto fail;

	Stream_Read_UINT16(s, pointer->xorBpp);
	Stream_Read_UINT16(s, pointer->cacheIndex); /* cacheIndex (2 bytes) */
	Stream_Read_UINT16(s, pointer->hotSpotX);   /* xPos (2 bytes) */
	Stream_Read_UINT16(s, pointer->hotSpotY);   /* yPos (2 bytes) */

	Stream_Read_UINT16(s, pointer->width);  /* width (2 bytes) */
	Stream_Read_UINT16(s, pointer->height); /* height (2 bytes) */

	if ((pointer->width > 384) || (pointer->height > 384))
		goto fail;

	Stream_Read_UINT16(s, pointer->lengthAndMask); /* lengthAndMask (2 bytes) */
	Stream_Read_UINT16(s, pointer->lengthXorMask); /* lengthXorMask (2 bytes) */

	if (pointer->hotSpotX >= pointer->width)
		pointer->hotSpotX = 0;

	if (pointer->hotSpotY >= pointer->height)
		pointer->hotSpotY = 0;

	if (pointer->lengthXorMask > 0)
	{
		/**
		 * Spec states that:
		 *
		 * xorMaskData (variable): A variable-length array of bytes. Contains the 24-bpp, bottom-up
		 * XOR mask scan-line data. The XOR mask is padded to a 2-byte boundary for each encoded
		 * scan-line. For example, if a 3x3 pixel cursor is being sent, then each scan-line will
		 * consume 10 bytes (3 pixels per scan-line multiplied by 3 bytes per pixel, rounded up to
		 * the next even number of bytes).
		 *
		 * In fact instead of 24-bpp, the bpp parameter is given by the containing packet.
		 */
		if (Stream_GetRemainingLength(s) < pointer->lengthXorMask)
			goto fail;

		scanlineSize = (7 + pointer->xorBpp * pointer->width) / 8;
		scanlineSize = ((scanlineSize + 1) / 2) * 2;

		if (scanlineSize * pointer->height != pointer->lengthXorMask)
		{
			WLog_ERR(TAG,
			         "invalid lengthXorMask: width=%" PRIu32 " height=%" PRIu32 ", %" PRIu32
			         " instead of %" PRIu32 "",
			         pointer->width, pointer->height, pointer->lengthXorMask,
			         scanlineSize * pointer->height);
			goto fail;
		}

		newMask = realloc(pointer->xorMaskData, pointer->lengthXorMask);

		if (!newMask)
			goto fail;

		pointer->xorMaskData = newMask;
		Stream_Read(s, pointer->xorMaskData, pointer->lengthXorMask);
	}

	if (pointer->lengthAndMask > 0)
	{
		/**
		 * andMaskData (variable): A variable-length array of bytes. Contains the 1-bpp, bottom-up
		 * AND mask scan-line data. The AND mask is padded to a 2-byte boundary for each encoded
		 * scan-line. For example, if a 7x7 pixel cursor is being sent, then each scan-line will
		 * consume 2 bytes (7 pixels per scan-line multiplied by 1 bpp, rounded up to the next even
		 * number of bytes).
		 */
		if (Stream_GetRemainingLength(s) < pointer->lengthAndMask)
			goto fail;

		scanlineSize = ((7 + pointer->width) / 8);
		scanlineSize = ((1 + scanlineSize) / 2) * 2;

		if (scanlineSize * pointer->height != pointer->lengthAndMask)
		{
			WLog_ERR(TAG, "invalid lengthAndMask: %" PRIu32 " instead of %" PRIu32 "",
			         pointer->lengthAndMask, scanlineSize * pointer->height);
			goto fail;
		}

		newMask = realloc(pointer->andMaskData, pointer->lengthAndMask);

		if (!newMask)
			goto fail;

		pointer->andMaskData = newMask;
		Stream_Read(s, pointer->andMaskData, pointer->lengthAndMask);
	}

	if (Stream_GetRemainingLength(s) > 0)
		Stream_Seek_UINT8(s); /* pad (1 byte) */

	return TRUE;
fail:
	return FALSE;
}