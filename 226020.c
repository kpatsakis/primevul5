wStream* cliprdr_packet_file_contents_request_new(const CLIPRDR_FILE_CONTENTS_REQUEST* request)
{
	wStream* s;

	if (!request)
		return NULL;

	s = cliprdr_packet_new(CB_FILECONTENTS_REQUEST, 0, 28);

	if (!s)
		return NULL;

	cliprdr_write_file_contents_request(s, request);
	return s;
}