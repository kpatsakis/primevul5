wStream* cliprdr_packet_file_contents_response_new(const CLIPRDR_FILE_CONTENTS_RESPONSE* response)
{
	wStream* s;

	if (!response)
		return NULL;

	s = cliprdr_packet_new(CB_FILECONTENTS_RESPONSE, response->msgFlags, 4 + response->cbRequested);

	if (!s)
		return NULL;

	cliprdr_write_file_contents_response(s, response);
	return s;
}