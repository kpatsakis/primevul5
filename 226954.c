static BOOL license_read_preamble(wStream* s, BYTE* bMsgType, BYTE* flags, UINT16* wMsgSize)
{
	/* preamble (4 bytes) */
	if (Stream_GetRemainingLength(s) < 4)
		return FALSE;

	Stream_Read_UINT8(s, *bMsgType);  /* bMsgType (1 byte) */
	Stream_Read_UINT8(s, *flags);     /* flags (1 byte) */
	Stream_Read_UINT16(s, *wMsgSize); /* wMsgSize (2 bytes) */
	return TRUE;
}