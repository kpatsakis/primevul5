static BOOL license_write_preamble(wStream* s, BYTE bMsgType, BYTE flags, UINT16 wMsgSize)
{
	if (!Stream_EnsureRemainingCapacity(s, 4))
		return FALSE;

	/* preamble (4 bytes) */
	Stream_Write_UINT8(s, bMsgType);  /* bMsgType (1 byte) */
	Stream_Write_UINT8(s, flags);     /* flags (1 byte) */
	Stream_Write_UINT16(s, wMsgSize); /* wMsgSize (2 bytes) */
	return TRUE;
}