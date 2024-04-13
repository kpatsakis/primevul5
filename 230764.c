static UINT32 update_glyph_offset(const BYTE* data, size_t length, UINT32 index, INT32* x, INT32* y,
                                  UINT32 ulCharInc, UINT32 flAccel)
{
	if ((ulCharInc == 0) && (!(flAccel & SO_CHAR_INC_EQUAL_BM_BASE)))
	{
		UINT32 offset = data[index++];

		if (offset & 0x80)
		{

			if (index + 1 < length)
			{
				offset = data[index++];
				offset |= ((UINT32)data[index++]) << 8;
			}
			else
				WLog_WARN(TAG, "[%s] glyph index out of bound %" PRIu32 " [max %" PRIuz "]", index,
				          length);
		}

		if (flAccel & SO_VERTICAL)
			*y += offset;

		if (flAccel & SO_HORIZONTAL)
			*x += offset;
	}

	return index;
}