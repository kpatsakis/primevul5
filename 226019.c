wStream* cliprdr_packet_lock_clipdata_new(const CLIPRDR_LOCK_CLIPBOARD_DATA* lockClipboardData)
{
	wStream* s;

	if (!lockClipboardData)
		return NULL;

	s = cliprdr_packet_new(CB_LOCK_CLIPDATA, 0, 4);

	if (!s)
		return NULL;

	cliprdr_write_lock_clipdata(s, lockClipboardData);
	return s;
}