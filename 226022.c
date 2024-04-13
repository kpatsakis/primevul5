static void cliprdr_write_file_contents_response(wStream* s,
                                                 const CLIPRDR_FILE_CONTENTS_RESPONSE* response)
{
	Stream_Write_UINT32(s, response->streamId); /* streamId (4 bytes) */
	Stream_Write(s, response->requestedData, response->cbRequested);
}