static int smtp_command_parse(struct smtp_command_parser *parser)
{
	const unsigned char *begin;
	size_t size, old_bytes = 0;
	int ret;

	while ((ret = i_stream_read_data(parser->input, &begin, &size,
					 old_bytes)) > 0) {
		parser->cur = begin;
		parser->end = parser->cur + size;

		ret = smtp_command_parse_line(parser);
		i_stream_skip(parser->input, parser->cur - begin);
		if (ret != 0)
			return ret;
		old_bytes = i_stream_get_data_size(parser->input);
	}

	if (ret == -2) {
		/* should not really happen */
		smtp_command_parser_error(parser,
			SMTP_COMMAND_PARSE_ERROR_LINE_TOO_LONG,
			"%s line is too long",
			(parser->auth_response ?
				"AUTH response" : "Command"));
		return -1;
	}
	if (ret < 0) {
		i_assert(parser->input->eof);
		if (parser->input->stream_errno == 0) {
			if (parser->state.state == SMTP_COMMAND_PARSE_STATE_INIT)
				ret = -2;
			smtp_command_parser_error(parser,
				SMTP_COMMAND_PARSE_ERROR_BROKEN_COMMAND,
				"Premature end of input");
		} else {
			smtp_command_parser_error(parser,
				SMTP_COMMAND_PARSE_ERROR_BROKEN_STREAM,
				"Stream error: %s",
				i_stream_get_error(parser->input));
		}
	}
	return ret;
}