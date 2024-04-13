static BOOL _update_read_pointer_color(wStream* s, POINTER_COLOR_UPDATE* pointer_color, BYTE xorBpp)
{
	BYTE* newMask;
	UINT32 scanlineSize;

	if (!pointer_color)
		goto fail;

	if (Stream_GetRemainingLength(s) < 14)
		goto fail;

	Stream_Read_UINT16(s, pointer_color->cacheIndex); /* cacheIndex (2 bytes) */
	Stream_Read_UINT16(s, pointer_color->xPos);       /* xPos (2 bytes) */
	Stream_Read_UINT16(s, pointer_color->yPos);       /* yPos (2 bytes) */
	/**
	 *  As stated in 2.2.9.1.1.4.4 Color Pointer Update:
	 *  The maximum allowed pointer width/height is 96 pixels if the client indicated support
	 *  for large pointers by setting the LARGE_POINTER_FLAG (0x00000001) in the Large
	 *  Pointer Capability Set (section 2.2.7.2.7). If the LARGE_POINTER_FLAG was not
	 *  set, the maximum allowed pointer width/height is 32 pixels.
	 *
	 *  So we check for a maximum of 96 for CVE-2014-0250.
	 */
	Stream_Read_UINT16(s, pointer_color->width);  /* width (2 bytes) */
	Stream_Read_UINT16(s, pointer_color->height); /* height (2 bytes) */

	if ((pointer_color->width > 96) || (pointer_color->height > 96))
		goto fail;

	Stream_Read_UINT16(s, pointer_color->lengthAndMask); /* lengthAndMask (2 bytes) */
	Stream_Read_UINT16(s, pointer_color->lengthXorMask); /* lengthXorMask (2 bytes) */

	/**
	 * There does not seem to be any documentation on why
	 * xPos / yPos can be larger than width / height
	 * so it is missing in documentation or a bug in implementation
	 * 2.2.9.1.1.4.4 Color Pointer Update (TS_COLORPOINTERATTRIBUTE)
	 */
	if (pointer_color->xPos >= pointer_color->width)
		pointer_color->xPos = 0;

	if (pointer_color->yPos >= pointer_color->height)
		pointer_color->yPos = 0;

	if (pointer_color->lengthXorMask > 0)
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
		if (Stream_GetRemainingLength(s) < pointer_color->lengthXorMask)
			goto fail;

		scanlineSize = (7 + xorBpp * pointer_color->width) / 8;
		scanlineSize = ((scanlineSize + 1) / 2) * 2;

		if (scanlineSize * pointer_color->height != pointer_color->lengthXorMask)
		{
			WLog_ERR(TAG,
			         "invalid lengthXorMask: width=%" PRIu32 " height=%" PRIu32 ", %" PRIu32
			         " instead of %" PRIu32 "",
			         pointer_color->width, pointer_color->height, pointer_color->lengthXorMask,
			         scanlineSize * pointer_color->height);
			goto fail;
		}

		newMask = realloc(pointer_color->xorMaskData, pointer_color->lengthXorMask);

		if (!newMask)
			goto fail;

		pointer_color->xorMaskData = newMask;
		Stream_Read(s, pointer_color->xorMaskData, pointer_color->lengthXorMask);
	}

	if (pointer_color->lengthAndMask > 0)
	{
		/**
		 * andMaskData (variable): A variable-length array of bytes. Contains the 1-bpp, bottom-up
		 * AND mask scan-line data. The AND mask is padded to a 2-byte boundary for each encoded
		 * scan-line. For example, if a 7x7 pixel cursor is being sent, then each scan-line will
		 * consume 2 bytes (7 pixels per scan-line multiplied by 1 bpp, rounded up to the next even
		 * number of bytes).
		 */
		if (Stream_GetRemainingLength(s) < pointer_color->lengthAndMask)
			goto fail;

		scanlineSize = ((7 + pointer_color->width) / 8);
		scanlineSize = ((1 + scanlineSize) / 2) * 2;

		if (scanlineSize * pointer_color->height != pointer_color->lengthAndMask)
		{
			WLog_ERR(TAG, "invalid lengthAndMask: %" PRIu32 " instead of %" PRIu32 "",
			         pointer_color->lengthAndMask, scanlineSize * pointer_color->height);
			goto fail;
		}

		newMask = realloc(pointer_color->andMaskData, pointer_color->lengthAndMask);

		if (!newMask)
			goto fail;

		pointer_color->andMaskData = newMask;
		Stream_Read(s, pointer_color->andMaskData, pointer_color->lengthAndMask);
	}

	if (Stream_GetRemainingLength(s) > 0)
		Stream_Seek_UINT8(s); /* pad (1 byte) */

	return TRUE;
fail:
	return FALSE;
}