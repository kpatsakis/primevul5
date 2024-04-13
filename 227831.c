static void rdp_capability_set_finish(wStream* s, UINT16 header, UINT16 type)
{
	size_t footer;
	size_t length;
	footer = Stream_GetPosition(s);
	length = footer - header;
	Stream_SetPosition(s, header);
	rdp_write_capability_set_header(s, (UINT16)length, type);
	Stream_SetPosition(s, footer);
}