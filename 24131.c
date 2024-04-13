smtp_command_parser_init(struct istream *input,
			 const struct smtp_command_limits *limits)
{
	struct smtp_command_parser *parser;

	parser = i_new(struct smtp_command_parser, 1);
	parser->input = input;
	i_stream_ref(input);

	if (limits != NULL)
		parser->limits = *limits;
	if (parser->limits.max_parameters_size == 0) {
		parser->limits.max_parameters_size =
			SMTP_COMMAND_DEFAULT_MAX_PARAMETERS_SIZE;
	}
	if (parser->limits.max_auth_size == 0) {
		parser->limits.max_auth_size =
			SMTP_COMMAND_DEFAULT_MAX_AUTH_SIZE;
	}
	if (parser->limits.max_data_size == 0) {
		parser->limits.max_data_size =
			SMTP_COMMAND_DEFAULT_MAX_DATA_SIZE;
	}

	return parser;
}