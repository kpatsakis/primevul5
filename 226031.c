cliprdr_packet_unlock_clipdata_new(const CLIPRDR_UNLOCK_CLIPBOARD_DATA* unlockClipboardData)
{
	wStream* s;

	if (!unlockClipboardData)
		return NULL;

	s = cliprdr_packet_new(CB_LOCK_CLIPDATA, 0, 4);

	if (!s)
		return NULL;

	cliprdr_write_unlock_clipdata(s, unlockClipboardData);
	return s;
}