bool smtp_command_parser_pending_data(struct smtp_command_parser *parser)
{
	if (parser->data == NULL)
		return FALSE;
	return i_stream_have_bytes_left(parser->data);
}