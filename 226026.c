static void cliprdr_write_file_contents_request(wStream* s,
                                                const CLIPRDR_FILE_CONTENTS_REQUEST* request)
{
	Stream_Write_UINT32(s, request->streamId);      /* streamId (4 bytes) */
	Stream_Write_UINT32(s, request->listIndex);     /* listIndex (4 bytes) */
	Stream_Write_UINT32(s, request->dwFlags);       /* dwFlags (4 bytes) */
	Stream_Write_UINT32(s, request->nPositionLow);  /* nPositionLow (4 bytes) */
	Stream_Write_UINT32(s, request->nPositionHigh); /* nPositionHigh (4 bytes) */
	Stream_Write_UINT32(s, request->cbRequested);   /* cbRequested (4 bytes) */

	if (request->haveClipDataId)
		Stream_Write_UINT32(s, request->clipDataId); /* clipDataId (4 bytes) */
}