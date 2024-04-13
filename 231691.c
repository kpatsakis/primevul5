static BOOL clear_decompress_nscodec(NSC_CONTEXT* nsc, UINT32 width, UINT32 height, wStream* s,
                                     UINT32 bitmapDataByteCount, BYTE* pDstData, UINT32 DstFormat,
                                     UINT32 nDstStep, UINT32 nXDstRel, UINT32 nYDstRel)
{
	BOOL rc;

	if (Stream_GetRemainingLength(s) < bitmapDataByteCount)
	{
		WLog_ERR(TAG, "stream short %" PRIuz " [%" PRIu32 " expected]",
		         Stream_GetRemainingLength(s), bitmapDataByteCount);
		return FALSE;
	}

	rc = nsc_process_message(nsc, 32, width, height, Stream_Pointer(s), bitmapDataByteCount,
	                         pDstData, DstFormat, nDstStep, nXDstRel, nYDstRel, width, height,
	                         FREERDP_FLIP_NONE);
	Stream_Seek(s, bitmapDataByteCount);
	return rc;
}