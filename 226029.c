static void cliprdr_write_unlock_clipdata(wStream* s,
                                          const CLIPRDR_UNLOCK_CLIPBOARD_DATA* unlockClipboardData)
{
	cliprdr_write_lock_unlock_clipdata(s, unlockClipboardData->clipDataId);
}