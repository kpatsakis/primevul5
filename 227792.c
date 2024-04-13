static BOOL rdp_read_pointer_capability_set(wStream* s, UINT16 length, rdpSettings* settings)
{
	UINT16 colorPointerFlag;
	UINT16 colorPointerCacheSize;
	UINT16 pointerCacheSize;

	if (length < 8)
		return FALSE;

	Stream_Read_UINT16(s, colorPointerFlag);      /* colorPointerFlag (2 bytes) */
	Stream_Read_UINT16(s, colorPointerCacheSize); /* colorPointerCacheSize (2 bytes) */

	/* pointerCacheSize is optional */
	if (length >= 10)
		Stream_Read_UINT16(s, pointerCacheSize); /* pointerCacheSize (2 bytes) */
	else
		pointerCacheSize = 0;

	if (colorPointerFlag == FALSE)
		settings->ColorPointerFlag = FALSE;

	if (settings->ServerMode)
	{
		settings->PointerCacheSize = pointerCacheSize;
	}

	return TRUE;
}