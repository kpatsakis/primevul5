wStream* cliprdr_packet_format_list_new(const CLIPRDR_FORMAT_LIST* formatList,
                                        BOOL useLongFormatNames)
{
	wStream* s;
	UINT32 index;
	int cchWideChar;
	LPWSTR lpWideCharStr;
	int formatNameSize;
	char* szFormatName;
	WCHAR* wszFormatName;
	BOOL asciiNames = FALSE;
	CLIPRDR_FORMAT* format;

	if (formatList->msgType != CB_FORMAT_LIST)
		WLog_WARN(TAG, "[%s] called with invalid type %08" PRIx32, __FUNCTION__,
		          formatList->msgType);

	if (!useLongFormatNames)
	{
		UINT32 length = formatList->numFormats * 36;
		s = cliprdr_packet_new(CB_FORMAT_LIST, 0, length);

		if (!s)
		{
			WLog_ERR(TAG, "cliprdr_packet_new failed!");
			return NULL;
		}

		for (index = 0; index < formatList->numFormats; index++)
		{
			size_t formatNameLength = 0;
			format = (CLIPRDR_FORMAT*)&(formatList->formats[index]);
			Stream_Write_UINT32(s, format->formatId); /* formatId (4 bytes) */
			formatNameSize = 0;

			szFormatName = format->formatName;

			if (asciiNames)
			{
				if (szFormatName)
					formatNameLength = strnlen(szFormatName, 32);

				if (formatNameLength > 31)
					formatNameLength = 31;

				Stream_Write(s, szFormatName, formatNameLength);
				Stream_Zero(s, 32 - formatNameLength);
			}
			else
			{
				wszFormatName = NULL;

				if (szFormatName)
					formatNameSize =
					    ConvertToUnicode(CP_UTF8, 0, szFormatName, -1, &wszFormatName, 0);

				if (formatNameSize < 0)
					return NULL;

				if (formatNameSize > 15)
					formatNameSize = 15;

				/* size in bytes  instead of wchar */
				formatNameSize *= 2;

				if (wszFormatName)
					Stream_Write(s, wszFormatName, (size_t)formatNameSize);

				Stream_Zero(s, (size_t)(32 - formatNameSize));
				free(wszFormatName);
			}
		}
	}
	else
	{
		UINT32 length = 0;
		for (index = 0; index < formatList->numFormats; index++)
		{
			format = (CLIPRDR_FORMAT*)&(formatList->formats[index]);
			length += 4;
			formatNameSize = 2;

			if (format->formatName)
				formatNameSize =
				    MultiByteToWideChar(CP_UTF8, 0, format->formatName, -1, NULL, 0) * 2;

			if (formatNameSize < 0)
				return NULL;

			length += (UINT32)formatNameSize;
		}

		s = cliprdr_packet_new(CB_FORMAT_LIST, 0, length);

		if (!s)
		{
			WLog_ERR(TAG, "cliprdr_packet_new failed!");
			return NULL;
		}

		for (index = 0; index < formatList->numFormats; index++)
		{
			format = (CLIPRDR_FORMAT*)&(formatList->formats[index]);
			Stream_Write_UINT32(s, format->formatId); /* formatId (4 bytes) */

			if (format->formatName)
			{
				const size_t cap = Stream_Capacity(s);
				const size_t pos = Stream_GetPosition(s);
				const size_t rem = cap - pos;
				if ((cap < pos) || ((rem / 2) > INT_MAX))
					return NULL;

				lpWideCharStr = (LPWSTR)Stream_Pointer(s);
				cchWideChar = (int)(rem / 2);
				formatNameSize = MultiByteToWideChar(CP_UTF8, 0, format->formatName, -1,
				                                     lpWideCharStr, cchWideChar) *
				                 2;
				if (formatNameSize < 0)
					return NULL;
				Stream_Seek(s, (size_t)formatNameSize);
			}
			else
			{
				Stream_Write_UINT16(s, 0);
			}
		}
	}

	return s;
}