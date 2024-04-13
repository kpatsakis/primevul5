smtp_command_parser_error(struct smtp_command_parser *parser,
			  enum smtp_command_parse_error code,
			  const char *format, ...)
{
	va_list args;

	parser->state.state = SMTP_COMMAND_PARSE_STATE_ERROR;

	i_free(parser->error);
	parser->error_code = code;

	va_start(args, format);
	parser->error = i_strdup_vprintf(format, args);
	va_end(args);
}