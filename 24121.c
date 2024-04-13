smtp_command_parse_finish_data(struct smtp_command_parser *parser)
{
	const unsigned char *data;
	size_t size;
	int ret;

	parser->error_code = SMTP_COMMAND_PARSE_ERROR_NONE;
	parser->error = NULL;

	if (parser->data == NULL)
		return 1;
	if (parser->data->eof) {
		i_stream_unref(&parser->data);
		return 1;
	}

	while ((ret = i_stream_read_data(parser->data, &data, &size, 0)) > 0)
		i_stream_skip(parser->data, size);
	if (ret == 0 || parser->data->stream_errno != 0) {
		switch (parser->data->stream_errno) {
		case 0:
			return 0;
		case EIO:
			smtp_command_parser_error(parser,
				SMTP_COMMAND_PARSE_ERROR_BROKEN_COMMAND,
				"Invalid command data");
			break;
		case EMSGSIZE:
			smtp_command_parser_error(parser,
				SMTP_COMMAND_PARSE_ERROR_DATA_TOO_LARGE,
				"Command data too large");
			break;
		default:
			smtp_command_parser_error(parser,
				SMTP_COMMAND_PARSE_ERROR_BROKEN_STREAM,
				"Stream error while skipping command data: "
				"%s", i_stream_get_error(parser->data));
		}
		return -1;
	}
	i_stream_unref(&parser->data);
	return 1;
}