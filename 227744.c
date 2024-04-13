static BOOL rdp_print_bitmap_cache_capability_set(wStream* s, UINT16 length)
{
	UINT32 pad1, pad2, pad3;
	UINT32 pad4, pad5, pad6;
	UINT16 Cache0Entries;
	UINT16 Cache0MaximumCellSize;
	UINT16 Cache1Entries;
	UINT16 Cache1MaximumCellSize;
	UINT16 Cache2Entries;
	UINT16 Cache2MaximumCellSize;
	WLog_INFO(TAG, "BitmapCacheCapabilitySet (length %" PRIu16 "):", length);

	if (length < 40)
		return FALSE;

	Stream_Read_UINT32(s, pad1);                  /* pad1 (4 bytes) */
	Stream_Read_UINT32(s, pad2);                  /* pad2 (4 bytes) */
	Stream_Read_UINT32(s, pad3);                  /* pad3 (4 bytes) */
	Stream_Read_UINT32(s, pad4);                  /* pad4 (4 bytes) */
	Stream_Read_UINT32(s, pad5);                  /* pad5 (4 bytes) */
	Stream_Read_UINT32(s, pad6);                  /* pad6 (4 bytes) */
	Stream_Read_UINT16(s, Cache0Entries);         /* Cache0Entries (2 bytes) */
	Stream_Read_UINT16(s, Cache0MaximumCellSize); /* Cache0MaximumCellSize (2 bytes) */
	Stream_Read_UINT16(s, Cache1Entries);         /* Cache1Entries (2 bytes) */
	Stream_Read_UINT16(s, Cache1MaximumCellSize); /* Cache1MaximumCellSize (2 bytes) */
	Stream_Read_UINT16(s, Cache2Entries);         /* Cache2Entries (2 bytes) */
	Stream_Read_UINT16(s, Cache2MaximumCellSize); /* Cache2MaximumCellSize (2 bytes) */
	WLog_INFO(TAG, "\tpad1: 0x%08" PRIX32 "", pad1);
	WLog_INFO(TAG, "\tpad2: 0x%08" PRIX32 "", pad2);
	WLog_INFO(TAG, "\tpad3: 0x%08" PRIX32 "", pad3);
	WLog_INFO(TAG, "\tpad4: 0x%08" PRIX32 "", pad4);
	WLog_INFO(TAG, "\tpad5: 0x%08" PRIX32 "", pad5);
	WLog_INFO(TAG, "\tpad6: 0x%08" PRIX32 "", pad6);
	WLog_INFO(TAG, "\tCache0Entries: 0x%04" PRIX16 "", Cache0Entries);
	WLog_INFO(TAG, "\tCache0MaximumCellSize: 0x%04" PRIX16 "", Cache0MaximumCellSize);
	WLog_INFO(TAG, "\tCache1Entries: 0x%04" PRIX16 "", Cache1Entries);
	WLog_INFO(TAG, "\tCache1MaximumCellSize: 0x%04" PRIX16 "", Cache1MaximumCellSize);
	WLog_INFO(TAG, "\tCache2Entries: 0x%04" PRIX16 "", Cache2Entries);
	WLog_INFO(TAG, "\tCache2MaximumCellSize: 0x%04" PRIX16 "", Cache2MaximumCellSize);
	return TRUE;
}