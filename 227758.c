static size_t rdp_capability_set_start(wStream* s)
{
	size_t header = Stream_GetPosition(s);
	Stream_Zero(s, CAPSET_HEADER_LENGTH);
	return header;
}