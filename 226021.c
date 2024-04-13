static void cliprdr_write_lock_clipdata(wStream* s,
                                        const CLIPRDR_LOCK_CLIPBOARD_DATA* lockClipboardData)
{
	cliprdr_write_lock_unlock_clipdata(s, lockClipboardData->clipDataId);
}